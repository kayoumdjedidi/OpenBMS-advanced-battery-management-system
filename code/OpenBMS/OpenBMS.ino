#include <string.h>
void inputTip(bool*, float*);  //loads push button inputs and sets state variables
void stopp();                  //stops all activity except for voltage measuring and displaying
void warning();                //illuminates warnLED in case of a warning/malfunction
void inputBatt(int*, float*);  //stores analog input voltage values from the battery pack
int maximum(float*);           //finds the index of a battery cell with the greatest voltage
void chargeBatt(float*);       //executes the process of balancing
int minimum(float*);           //finds the index of a battery cell with the lowest voltage
void dischargeBatt(float*);    //executes the process of debalancing
void storage();                //prepares the battery pack for permanent storage
void warnCH(int);              //illuminates warnLED with 50% duty cyle (balance mode)
void warnSTBY();               //intermittently illuminates warnLED with short impulses
void warnDIS(int);             //intermittently illuminates warnLED with long impulses
void displayy();               //outputs parameters onto an LCD screen
void I2C_out(unsigned char);   //sends I2C data towards LCD screen
void I2C_Start();              //initiates I2C data transfer towards LCD screen
void I2C_Stop();               //stops I2C data transfer towards LCD screen
void Show(unsigned char*);     //forwards the message towards function "I2C_out"
void nextline1();              // sets cursor to line 1 on the LCD display
void nextline2();              //sets cursor to line 2 on the LCD display
void init_LCD();               //initializes the LCD display


int i, j;
int tCH, tST, tDIS;                        //variables for push button digital values
float CELL[6];                             //variables for battery cell voltage analog values
bool state[] = { 0, 1, 0, 1, 0 };          //variable for system state/mode {CH, ST, DIS, warning, storage}
unsigned long times1, times2, times3 = 0;  //variables for the delay workaround (system state/mode)
unsigned long wait1, wait2, wait3 = 0;     //variables for the delay workaround (system state/mode)
unsigned long times4, wait4 = 0;           //variables for the delay workaround (safety mode)
unsigned long times5, wait5 = 0;           //variables for the delay workaround (permanent storage mode)
//
unsigned long timesB, waitB = 0;  //variables for the delay workaround (CH&DIS)
//
unsigned long timesA, waitA = 0;  //variables for the delay workaround (warnLED)
unsigned long timesF, waitF = 0;  
unsigned long counter = 0;        //variable for LED counter
bool LEDstate = 1;                //variable for state of the LED
//
bool DIGstate = 0;  //variable for balancing channel state
int index;          //variable for battery cell choice (when balancing/debalancing)
//
bool warningstate = 0;  //variable for safety mode state
int counterACH = 0;     //variable for safety mode counter
//
int counterB = 0;      //variable for debalance counter
int counterStore = 0;  //variable for counter of battery cell voltages greater than cellSTORE
//
unsigned long timesC, waitC = 0;  //variables for the delay workaround (display)
unsigned long timesD, waitD = 0;  //variables for the service menu
unsigned long timesE, waitE = 0;  //variables for LCD display backlight pause during safety mode toggle
//
const char Slave = 0x7C;           //variable for the LCD display
const char Comsend = 0x00;         
const char Datasend = 0x40;        
const char Line1 = 0x02;           
const char Line2 = 0xC0;           
int counterCell = 1;               //variable for battery cell order
bool change = 0;                   //variable for indication of safety mode toggle
char s1[18] = "Passive BMS M.K.";  //LCD intro string (line 1)
char s2[18] = "FERIT 2022      ";  //LCD intro string (line 2)
char s11[18] = "\0";               //variable for intro strings output sign by sign
char a[18] = "\0";                 //string for outputting the first line on the LCD display
char b[18] = "\0";                 //string for outputting the second line on the LCD display
char c[3], d[5] = "\0";            //auxiliary strings for displayy function
//
#define pause1 1000   //press and hold time for balance and debalance modes
#define pause2 10000  //balancing time interval
#define pause3 5000 
#define pause4 5000  //debalancing time interval
#define pause5 30     //warnLED impulse interval, type 2
#define pause6 2500   //warnLED impulse interval, type 3
#define pause7 75     //warnLED impulse interval, type 4
#define pause8 250    //LCD display update rate; backlight pause during safety mode toggle
#define pause9 2000   //vrijeme cekanja za otvaranje servisnog menija
//
#define Vdd 5.025         //real '+5V' bus voltage value
#define cellMAX 4.2       //maximum voltage per Liion battery cell
#define cellMIN 3.7       //minimum voltage per Liion battery cell
#define cellSTORE 3.8     //optimal voltage per Liion battery cell when permanently stored
#define tempMAX 60        //maximum battery pack temperature
#define tempMIN 5         //minimum battery pack temperature
#define correct 1.009     //measured voltage value compensation factor for cell 5
#define TEXT_LCD_SIZE 18  //LCD display font size
#define brightness 128    //LCD display backlight intensity (0255)

int BATT[6] = { A7, A6, A3, A2, A1, A0 };  //declaration of battery pack input pins (cell 1  cell 5, NTC)
                                           //BATT[0] => NTC, BATT[1] => cell 1 ... BATT[5] => cell 5
//
int DIG[6] = { 100, 6, 5, 4, 3, 2 };  //declaration of digital output pins for optocouplers
                                      //DIG[0] => not functional, DIG[1] => cell 1 ... DIG[5] => cell 5
//
int WARN = 11;  //declaration of warnLED pin
//
int TIP[4] = { 101, 7, 8, 9 };  //declaration of push button input digital pins
                                //TIP[0] => not functional, TIP[1] = BALANCE ... TIP[3] = DEBALANCE
//
#define LIGHT 10  //declaration of LCD display backlight pin
#define SDA A4    //declaration of LCD display data pin
#define SCL A5    //declaration of LCD display clock pin
#define RES 3     //declaration of LCD display reset pin

void setup() {

  for (i = 0; i < 6; i++) {  
    pinMode(BATT[i], INPUT);
  }
  //
  for (i = 1; i < 6; i++) {  
    digitalWrite(DIG[i], LOW); 
  }
  //
  pinMode(WARN, OUTPUT);    
  analogWrite(WARN, LOW);   
                             
  for (i = 1; i < 4; i++) {  
    pinMode(TIP[i], INPUT);
  }
  //
  pinMode(LIGHT, OUTPUT);             
  for (i = 0; i < brightness; i++) {  
    analogWrite(LIGHT, i);
    delay(10);
  }
  analogWrite(LIGHT, brightness);
  pinMode(SDA, OUTPUT);  
  pinMode(SCL, OUTPUT);  
  pinMode(RES, OUTPUT);  
                         
  init_LCD();            
  for (i = 0; i < 20; i++) {
    s11[i] = s1[i];
    delay(50);
    nextline1();
    Show(s11);
  }
  delay(300);
  nextline2();
  Show(s2);
  delay(2000);
  //
  Serial.begin(9600);
  //
}

void loop() {

  inputTip(&state[0], &CELL[0]);
  displayy();
  //
  if (state[0] == 1) {
    warnCH();
    chargeBatt(&CELL[0]);
  }
  if (state[1] == 1 && warningstate == 0) {
    stopp();
    if (((tCH == HIGH && tDIS == HIGH) || (tCH == LOW && tST == LOW) || (tDIS == LOW && tST == LOW))) {
      warnSTBY();
    }
    inputBatt(&CELL[0]);
  }
  if (state[2] == 1) {
    warnDIS();
    dischargeBatt(&CELL[0]);
  }
  if (warningstate == 1) {
    warning();
  }
  if (state[4] == 1) {
    storage();
  }
}

void inputTip(bool* state, float* CELL) {

  tCH = digitalRead(TIP[1]);
  tST = digitalRead(TIP[2]);
  tDIS = digitalRead(TIP[3]);
  //
  inputBatt(&CELL[0]);
  //
  times1 = millis();  //balance mode turn on by long pressing push button
  if ((tCH == HIGH) || (tDIS == LOW && tCH == LOW) || (tCH == LOW && tST == LOW)) {
    wait1 = times1;
  }
  if (((times1 - wait1) >= pause1) && (state[1] == 1) && (tDIS == HIGH) && (warningstate == 0)) {
    state[0] = 1;
    state[1] = 0;
    state[2] = 0;
    state[4] = 0;
    counter = 0;
    counterB = 0;
    waitA = timesA;
    waitB = timesB;
    wait1 = times1;
    wait2 = times2;
    wait3 = times3;
  }
  //
  times2 = millis();  //turn off all activity except for voltage measuring and displaying
  if ((tST == HIGH) || (tCH == LOW && tST == LOW && tDIS == LOW)) {
    wait2 = times2;
  }
  if (state[4] == HIGH) {
    if (((times2 - wait2) >= (pause1 / 10))) {
      state[0] = 0;
      state[1] = 1;
      state[2] = 0;
      state[4] = 0;
      warningstate = 0;
      counter = 0;
      DIGstate = 0;
    }
  } else if (((times2 - wait2) >= 20)) {
    state[0] = 0;
    state[1] = 1;
    state[2] = 0;
    state[4] = 0;
    warningstate = 0;
    counter = 0;
    DIGstate = 0;
  }
  //
  times3 = millis();  //debalance mode turn on by long pressing push button
  if ((tDIS == HIGH) || (tDIS == LOW && tCH == LOW) || (tDIS == LOW && tST == LOW)) {
    wait3 = times3;
  }
  if ((times3 - wait3) >= pause1 && state[1] == 1 && tCH == HIGH && warningstate == 0) {
    state[0] = 0;
    state[1] = 0;
    state[2] = 1;
    state[4] = 0;
    counter = 0;
    counterB = 0;
    waitA = timesA;
    waitB = timesB;
    wait1 = times1;
    wait2 = times2;
    wait3 = times3;
  }
  //
  times4 = millis();  //turn on safety mode by long pressing push buttons (balance + debalance)
  if (!((tCH == LOW) && (tDIS == LOW) && (tST == HIGH))) {
    wait4 = times4;
  }
  if ((times4 - wait4) > (3 * pause1)) {
    wait4 = times4;
    state[3] = !state[3];
    warningstate = 0;
    change = 1;
    digitalWrite(WARN, LOW);
    analogWrite(LIGHT, 0);
  }
  //
  times5 = millis();  //turn on permanent storage mode by long pressing all three push buttons
  if (!((tCH == LOW) && (tST == LOW) && (tDIS == LOW))) {
    wait5 = times5;
  }
  if (((times5  - wait5) > (3 * pause1)) && warningstate == 0) {
    wait5 = times5;
    state[0] = 0;
    state[1] = !state[1];
    state[2] = 0;
    state[4] = !state[4];
    counter = 0;
    counterB = 0;
    waitA = timesA;
    waitB = timesB;
    wait1 = times1;
    wait2 = times2;
    wait3 = times3;
  }
}

void stopp() {
  for (i = 1; i < 6; i++) {
    digitalWrite(DIG[i], LOW);
  }
  //
}

void warning() {

  unsigned long timesF = millis();
  //
  stopp();
  //
  if ((timesF - waitF) >= pause7) {  //warnLED flashing
    waitF = timesF;
    LEDstate = !LEDstate;
    digitalWrite(WARN, LEDstate);
  }
}

void inputBatt(float* CELL) {

  CELL[0] = analogRead(BATT[0]);             //battery pack temperature measurement value mapping
  CELL[0] = map(CELL[0], 350, 570, 48, 15);  //    =>    MODIFY THIS VARIABLE
                                             //
  CELL[1] = analogRead(BATT[1]);             //battery cell 1 measuring
  CELL[1] = CELL[1] * (Vdd / 1023);
  //
  CELL[2] = analogRead(BATT[2]);  //battery cell 2 measuring
  CELL[2] = CELL[2] * (Vdd / 1023);
  //
  CELL[3] = analogRead(BATT[3]);  //battery cell 3 measuring
  CELL[3] = CELL[3] * (Vdd / 1023);
  //
  CELL[4] = analogRead(BATT[4]);  //battery cell 4 measuring
  CELL[4] = CELL[4] * (Vdd / 1023);
  //
  CELL[5] = analogRead(BATT[5]);  //battery cell 5 measuring
  CELL[5] = (CELL[5] * (Vdd / 1023)) * correct;
  //
  if (state[3] == 1) {          //error triggering algorithm
    counterACH = 0;             //(triggered if the voltages of *ALL* cells are less than 3.7V
    for (i = 1; i < 6; i++) {   //or if *ANY* of cell's voltages is greater than 4.2V
      if (CELL[i] < cellMIN) {  //or if battery pack temperature value exceeds its rated interval)
        counterACH++;
      }
    }
    for (i = 1; i < 6; i++) {
      if ((CELL[i] >= cellMAX) || (counterACH == 5) || (CELL[0] > tempMAX) || (CELL[0] < tempMIN)) {
        state[0] = 0;
        state[1] = 1;
        if (warningstate == 0) {
          warningstate = 1;
        }
        state[2] = 0;
        counter = 0;
        waitA = timesA;
        waitB = timesB;
        wait1 = times1;
        wait2 = times2;
        wait3 = times3;
      }
    }
  }
}

int maximum(float* CELL) {

  float maxi = 0;
  counterStore = 0;
  //
  for (i = 1; i < 6; i++) {  //counting cells that can be discharged safely
    if (CELL[i] > cellMIN) {
      counterStore++;
    }
  }
  //
  if (counterStore > 0) {
    for (i = 1; i < 6; i++) {  //finding index of a battery cell with the greatest voltage
      if (CELL[i] > maxi) {
        maxi = CELL[i];
        index = i;
      }
    }
  }
  //
  if (counterStore == 0) {  //returning to standby mode
    state[0] = 0;
    state[1] = 1;
  }
  //
  return index;
}

void chargeBatt(float* CELL) {
  unsigned long timesB = millis();
  //
  inputBatt(&CELL[0]);
  //
  if (counterB == 0) {  //initialization of balancing mode
    index = maximum(&CELL[0]);
    digitalWrite(DIG[index], HIGH);
    DIGstate = 1;
    waitB = timesB;
    counterB++;
  }
  //
  if ((timesB - waitB) >= pause2) {  //balancing mode algorithm
    DIGstate = 0;
  }
  if ((timesB - waitB) >= (pause2 + pause3)) {
    index = maximum(&CELL[0]);
    DIGstate = 1;
    waitB = timesB;
  }
  //
  digitalWrite(DIG[index], DIGstate);
}

int minimum(float* CELL) {
  float mini = 10;
  //
  for (i = 1; i < 6; i++) {  //finding index of a battery cell with the lowest voltage
    if (CELL[i] < mini && CELL[i] > cellMIN) {
      mini = CELL[i];
      index = i;
    }
    if (CELL[i] < cellMIN) {
      state[1] = 1;
      state[2] = 0;
      digitalWrite(WARN, HIGH);
    }
  }
  //
  return index;
}

void dischargeBatt(float* CELL) {
  unsigned long timesB = millis();
  //
  inputBatt(&CELL[0]);
  //
  if (counterB == 0) {  //initialization of debalancing mode
    index = minimum(&CELL[0]);
    digitalWrite(DIG[index], HIGH);
    DIGstate = 1;
    waitB = timesB;
    counterB++;
  }
  //
  if ((timesB - waitB) >= pause3) {  //debalancing mode algorithm
    DIGstate = 0;
  }
  if ((timesB - waitB) >= pause3 + pause4) {
    waitB = timesB;
    index = minimum(&CELL[0]);
    DIGstate = 1;
  }
  //
  digitalWrite(DIG[index], DIGstate);
}

void storage() {

  counterStore = 0;
  //
  for (i = 1; i < 6; i++) {  //counting cells that can be discharged safely
    if (CELL[i] > cellSTORE) {
      counterStore++;
    }
  }
  //
  if (counterStore > 0) {  //safe discharge process
    chargeBatt(&CELL[0]);
  }
  //
  if (counterStore == 0) {  //returning to standby mode
    state[4] = 0;
    state[1] = 1;
  }
}

void warnCH() {

  unsigned long timesA = millis();
  //
  if (counter == 0) {  //warnLED initialization
    digitalWrite(WARN, HIGH);
    counter++;
  }
  //
  if ((timesA - waitA) >= pause4) {  //warnLED flashing at "low" frequency at 50% duty cycle
    waitA = timesA;
    LEDstate = !LEDstate;
    digitalWrite(WARN, LEDstate);
  }
}

void warnSTBY() {
  unsigned long timesA = millis();
  //
  do {  //warnLED initialization
    digitalWrite(WARN, LEDstate);
    LEDstate = 1;
    counter++;
  } while (counter = 0);
  //
  if ((timesA - waitA) >= pause5) {  //warnLED flashing at "low" frequency and "low" duty cycle
    LEDstate = !LEDstate;
    digitalWrite(WARN, LOW);
  }
  if ((timesA - waitA) >= (pause5 + pause6)) {
    waitA = timesA;
    LEDstate = !LEDstate;
    digitalWrite(WARN, HIGH);
  }
}

void warnDIS() {

  unsigned long timesA = millis();
  //
  do {  //warnLED initialization
    digitalWrite(WARN, LEDstate);
    LEDstate = 1;
    counter++;
  } while (counter = 0);
  //
  if ((timesA - waitA) >= pause6) {  //warnLED flashing at "low" frequency and "high" duty cycle

    LEDstate = !LEDstate;
    digitalWrite(WARN, LEDstate);
  }
  if ((timesA - waitA) >= ((pause5 * 10) + pause6)) {
    waitA = timesA;

    LEDstate = !LEDstate;
    digitalWrite(WARN, LEDstate);
  }
}

void displayy() {
  unsigned long timesC = millis();
  unsigned long timesD = millis();
  unsigned long timesE = millis();
  //
  if (change == 0) {  //safety mode toggle indication (on/off)  temporary brightness reduction
    waitE = timesE;
  }
  if ((timesE - waitE) > pause8) {
    analogWrite(LIGHT, brightness);
    change = 0;
  }
  //
  if (warningstate == 1) {  //output message: "WARNING!!!  ERROR!!!"
    nextline1();
    Show("   WARNING!!!   ");
    nextline2();
    Show("    ERROR!!!    ");
    waitD = timesD;
  }
  //
  else {  //Service menu parameters output
    if ((!(tCH == HIGH && tST == LOW && tDIS == HIGH))) {
      waitD = timesD;
    }
    if (((timesD -  waitD) >= pause9)) {
      a[0] = 0;
      b[0] = 0;
      strcat(a, "    Temp:");
      if (CELL[0] < (5)) {
        c[0] = 0;
        strcat(c, "??");
      } else {
        dtostrf(CELL[0], 2, 0, c);
      }
      strcat(a, c);
      strcat(a, "C    ");
      strcat(b, "Safety mode: ");
      if (state[3] == 1) {
        strcat(b, "ON ");
      } else {
        strcat(b, "OFF");
      }
    }
    //
    else {  //main parameters output
      a[0] = 0;
      b[0] = 0;
      for (counterCell = 1; counterCell < 4; counterCell++) {
        if (CELL[counterCell] < 2) {
          d[0] = 0;
          strcat(d, "?v??");
        } else {
          dtostrf(CELL[counterCell], 4, 2, d);
          if ((DIGstate == HIGH) && (counterCell == index)) {
            d[1] = 'V';
          } else {
            d[1] = 'v';
          }
        }
        strcat(a, d);
        strcat(a, "  ");
      }
      for (counterCell = 4; counterCell < 6; counterCell++) {
        if (CELL[counterCell] < 2) {
          d[0] = 0;
          strcat(d, "?v??");
        } else {
          dtostrf(CELL[counterCell], 4, 2, d);
          if ((DIGstate == HIGH) && (counterCell == index)) {
            d[1] = 'V';
          } else {
            d[1] = 'v';
          }
        }
        strcat(b, d);
        strcat(b, "  ");
        if (counterCell == 5) {
          if (state[0] == HIGH) {
            strcat(b, " BA ");
          }
          if (state[1] == HIGH) {
            strcat(b, " ST ");
          }
          if (state[2] == HIGH) {
            strcat(b, "DEBA");
          }
          if (state[4] == HIGH) {
            strcat(b, "PERM");
          }
        }
      }
    }
    //
    if ((timesC  - waitC) >= pause8) {  //showing both lines on the LCD display
      waitC = timesC;
      nextline1();
      Show(a);
      nextline2();
      Show(b);
    }
  }
}

void I2C_out(unsigned char j) {

  int n;
  unsigned char d;
  d = j;
  //
  for (n = 0; n < 8; n++) {
    if ((d & 0x80) == 0x80) {
      digitalWrite(SDA, HIGH);
    } else {
      digitalWrite(SDA, LOW);
    }
    d = (d << 1);
    digitalWrite(SCL, LOW);
    digitalWrite(SCL, HIGH);
    digitalWrite(SCL, LOW);
  }
  digitalWrite(SCL, HIGH);
  while (SDA == HIGH) {
    digitalWrite(SCL, LOW);
    digitalWrite(SCL, HIGH);
  }
  digitalWrite(SCL, LOW);
}


void I2C_Start(void) {
  digitalWrite(SCL, HIGH);
  digitalWrite(SDA, HIGH);
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
}


void I2C_Stop(void) {
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
  digitalWrite(SCL, HIGH);
  digitalWrite(SCL, HIGH);
}


void Show(unsigned char* text) {
  int n;
  //
  I2C_Start();
  I2C_out(Slave);
  I2C_out(Datasend);
  for (n = 0; n < 16; n++) {
    I2C_out(*text);
    ++text;
  }
  I2C_Stop();
}


void nextline1(void) {

  I2C_Start();
  I2C_out(Slave);
  I2C_out(Comsend);
  I2C_out(Line1);
  I2C_Stop();
}

void nextline2(void) {
  I2C_Start();
  I2C_out(Slave);
  I2C_out(Comsend);
  I2C_out(Line2);
  I2C_Stop();
}

void init_LCD() {

  I2C_Start();
  I2C_out(Slave);
  I2C_out(Comsend);
  //
  I2C_out(0x38);
  I2C_out(0x39);
  I2C_out(0x14);
  I2C_out(0x78);
  I2C_out(0x5E);  
  I2C_out(0x6B);  
  I2C_out(0x0C);
  I2C_out(0x01);
  I2C_out(0x06);
  //
  I2C_Stop();
  I2C_out(0x01);
}
