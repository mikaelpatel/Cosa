Che Cosa?
====

Cosa is an object-oriented platform for Arduino that supports an event-driven programming paradigm with simple finite state machines. It contains a rich set of classes to support rapid prototyping of Internet-of-Things devices. A class overview may be found in CLASS.txt.

Cosa::Ciao is a data streaming format for encodes C/C++ language data types such as strings, integer and floating pointer numbers into a binary format. It that may be used for a number of applications; tracing, remote procedure calls, data exchange between Arduino devices,etc. The format allows user data types to be defined and values exchanged without additional encoding. The stream header itself is a pre-defined serializable data type. See CIAO.txt for more details and the example code (CosaCiaoPoint).

Cosa supports the following AVR/Atmega328P internal hardware modules; all pin modes, interrupt pins, PWM, Watchdog, UART (transmit), SPI and TWI. 

The primary programming paradigm is object-oriented and state-machine/event driven. Please see the example sketches in the directory Cosa/examples.

There is a large number of device drivers available for SPI, TWI and OneWire. A strict directory structure is used to organize the Cosa/driver source code. Sub-directories are used for each driver type. Please check the class overview for a full list of drivers.

To install download and move the Cosa directory to the Arduino cores directory within arduino itself, i.e., your-arduino-install-directory/hardware/arduino/cores/arduino.The examples should be moved to your sketchbook directory. 

Cosa uses the Arduino IDL and build system. Cosa classes are included with prefix, e.g. "Cosa/FileName.h". It is possible to use both Arduino and Cosa functions together, though in some cases the Cosa objects may become inconsistent.

Please note that documentation for each device driver hardware module may be found in the Cosa/references directory. 

The goal of this project is to provide an efficient programming platform for rapid prototyping of "Internet-of-things"-devices. Unfortunately Cosa is not a beginners entry level programming platform, though following some of the design patterns in Cosa will help beginners build more complex small scale embedded systems with richer concurrency and low power consumption.

Note:
1. Atmega328P based Arduino boards (Uno, Mini, Nano, LilyPad, etc) are supported. 
2. Timers, EEPROM, UART (receiver) are not supported but planned.
3. Arduino Mega, Leonardo, etc, are not currently supported but planned.
4. Attiny devices are not currently supported but planned.

Drivers:
1. DS18B20 Programmable Resolution 1-Wire Digital Thermometer.
2. AT24CXX Serial EEPROM.
3. DS1307 Realtime clock with RAM,
4. PCF8591 2-Wire 8-bit A/D and D/A converter.
5. ADXL345 Digital Accelerometer.
6. nRF24L01 Single Chip 2.4GHz Transceiver. 
7. DHT11 Humidity & Temperature Sensor device driver


