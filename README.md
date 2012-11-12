Cosa
====

Object-oriented platform for Arduino. Class hierarchy for Pins, Watchdog and SPI to start with. Supports device drivers for additional hardware abstraction and event driven programming for low power. The Pins class hierarchy allows declaration of sketch pin typing and usage. The Pins classes captures the Arduino/AVR pin types; input, pullup, interrupt, output, analog and pwm. The watchdog is used as the basic timer. An SPI class allows both slave and master mode device drivers for hardware on the SPI bus. To demonstrate usage there are two drivers; ADXL345 Accelerometer and NRF24L01+ Transceiver. Please see example code.

To install download and move the Cosa directory to the Arduino cores directory within arduino itself, i.e., your-arduino-install-directory/hardware/arduino/cores/arduino.

The examples should be moved to your sketchbook directory. 

Cosa classes are included with:

#include "Cosa/FileName.h"

For now the target architecture is Arduino based on Atmega328P; Arduino Uno, Nano, etc.

Additional classes are planned; Timers, TWI, UART, etc. And more drivers for typical modules. 

The target for Cosa is "Internet-of-things" and ultra low power and cost sensor networks.


