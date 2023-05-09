# OpenBMS-advanced-battery-management-system

Battery Management Systems (BMS) are electronic systems designed to manage and
monitor batteries, ensuring their optimal performance and safety. BMS are widely used
in various applications, including electric vehicles, renewable energy systems, and portable
electronics, to name a few.

The primary functions of a BMS are to protect the battery from overcharging, over-discharging,
and overheating, which can lead to a decrease in battery life and, in extreme cases, pose a safety
hazard. A BMS achieves this by monitoring various battery parameters, such as voltage, and
current, and taking appropriate actions based on the measured values.
BMSs come in various types, including passive, active, and hybrid. Passive BMSs rely on
passive balancing techniques to balance the individual cells in a battery pack, while active
BMSs use active balancing techniques. Hybrid BMSs combine the advantages of both passive
and active balancing techniques.

In recent years, there has been a growing interest in open-source BMSs, as they offer greater
transparency, flexibility, and cost-effectiveness compared to proprietary solutions. Open-source
BMSs allow users to modify and customize the system according to their needs and provide a
platform for collaboration and knowledge-sharing among the maker community.
The purpose of this report is to document the process of designing, developing, and testing the
Open Source BMS System. The report will cover the various aspects of the project, including
the design and development of the BMS PCB using Altium Design, and coding of the BMS
using Arduino. The report will also provide a comparison of the Open Source BMS System
with other commercial BMS systems, as well as its limitations and future improvements.
The report aims to serve as a guide for anyone interested in designing and developing their own
BMS system, as well as to promote the use of Open Source hardware and software.


The Open Source Battery Management System (OpenBMS) for Lithium Cells is a system
designed to Monitor battery status and display it, charge the battery as required, and most
importantly, balance the cells, insuring a longer lifespan, as well as protecting them from undervolting. The system can also prepare the cells for permanent storage.

OpenBMS is suitable for Lithium-ion, Lithium Iron Phosphate (Lifepo4), Nickel Cadmium
(NiCd), Nickel-Metal Hydride (NiMH), Lead Acid (flooded, VRLA, SLA) batteries up to 12V,
and other types such as NiZn, alkaline, etc., with a total voltage of up to 20V.A
The Open Source BMS System consists of several modules, including the Cell Module,
containing the voltage measure, balance, and charge circuits, the Limiter, a display Module in
the form of an i2c LCD display, and a control module in the form of a power regulator and
MCU.

The Cell Module is responsible for monitoring the voltage of each cell in the battery pack, and
the Control Unit processes this information and controls the operation of the Limiter and
charging. The Limiter limits the charging current to 1A when a cell in the pack is balancing,
and the Charging Module charges the battery pack according to the required battery parameters.
The PCB design was created using Altium Design, and is modular, allowing for the addition or
removal of cells as required. The board is double-layered and uses CEM methods, making it
suitable for use with a range of battery types.
The BMS system is powered by an Arduino Nano controller and is programmed using Arduino.
The system constantly monitors battery parameters, and instantly cuts off the input or output
from the battery as soon as any unusual behavior is detected. The system also includes warning
LEDs and charge indicator LEDs for user convenience.
