# OpenBMS-advanced-battery-management-system

# OpenBMS: Open Source advanced Battery Management System

OpenBMS is an open source battery management system (BMS) for lithium-ion and other types of batteries up to 12V and 20V total voltage. The system monitors battery status, charges the battery as required, and most importantly, balances the cells to ensure longer lifespan and protection against undervolting. The system is suitable for various applications, including electric vehicles, renewable energy systems, and portable electronics.

## Table of Contents

- [Introduction](#openbms-open-source-battery-management-system)
- [Table of Contents](#table-of-contents)
- [Features](#features)
- [Documentation](#documentation)
- [Hardware Design](#hardware-design)
- [Software Design](#software-design)
- [Limitations and Future Improvements](#limitations-and-future-improvements)
- [License](#license)

## Features

- Cell voltage measurement, balancing, and charging circuits
- Limiter to limit the charging current to 1A when a cell is balancing
- Display module with an i2c LCD display
- Control module with a power regulator and an Arduino Nano controller
- Support for various types of batteries up to 12V and 20V total voltage
- Modular design for easy addition or removal of cells
- Double-layered PCB design using CEM methods
- Instant cut-off of input or output from the battery upon detecting unusual behavior
- Warning LEDs and charge indicator LEDs for user convenience

## Documentation

For more detailed information about the OpenBMS project, please refer to the following resources:

- [Project report in PDF format](https://github.com/kayoumdjedidi/OpenBMS-advanced-battery-management-system/blob/main/Documents/Report.pdf)
- [Schematics, BOM, and Gerber files](https://github.com/kayoumdjedidi/OpenBMS-advanced-battery-management-system/tree/main/Hardware/PCB)
- [Arduino code](https://github.com/kayoumdjedidi/OpenBMS-advanced-battery-management-system/tree/main/Software/Arduino)

## Hardware Design

The OpenBMS PCB design was created using Altium Designer and is modular, allowing for the addition or removal of cells as required. The board is double-layered and uses CEM methods, making it suitable for use with a range of battery types.

## Software Design

The OpenBMS system is powered by an Arduino Nano controller and is programmed using Arduino. The system constantly monitors battery parameters and instantly cuts off the input or output from the battery as soon as any unusual behavior is detected. The system also includes warning LEDs and charge indicator LEDs for user convenience.

## Limitations and Future Improvements

The OpenBMS project is a work in progress, and there are several limitations and future improvements to consider, such as:

- The system currently supports up to 12V and 20V total voltage, but it could be expanded to support higher voltages.
- The charging module could be improved to provide more flexibility in charging parameters.
- The display module could be enhanced to provide more detailed information about battery status.
- The code could be optimized for better performance and efficiency.

## License

The OpenBMS project is released under the [MIT License](https://github.com/kayoumdjedidi/OpenBMS-advanced-battery-management-system/blob/main/LICENSE). You are free to use, modify, and distribute the project as long as you include the license file in your distribution.

