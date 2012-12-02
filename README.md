Che Cosa?
====

Cosa is an Object-oriented platform for Arduino. It contains a rich set of classes to support rapid prototyping of Internet-of-Things devices. A class overview may be found in the CLASS.txt file. A data streaming format is also defined; Ciao. This format encodes C/C++ language data types such as integer and floating pointer numbers into a binary format that may be used for a number of applications; tracing, remote procedure calls, etc. The format allows user defined data types to be defined and values exchanged without additional encoding. The stream header itself is a pre-defined serializable data type. 

Cosa supports the following AVR/Atmega328P internal hardware modules; all pin modes, interrupt pins, PWM, Watchdog, UART (transmit), SPI and TWI. The Watchdog is used as the primary even clock and allows low power event wait.

The primary programming paradigm is object-oriented, state-machine/event driven.

There is a large number of device drivers available for SPI, TWI and OneWire. A strict directory structure is used to organize the source code. Sub-directories are used for each driver type. Please check the class overview for a full list of drivers.

To install download and move the Cosa directory to the Arduino cores directory within arduino itself, i.e., your-arduino-install-directory/hardware/arduino/cores/arduino.The examples should be moved to your sketchbook directory. 

Cosa uses the Arduino IDL and build system. Cosa classes are included with prefix, e.g. "Cosa/FileName.h". It is possible to use both Arduino and Cosa functions together. Though in some cases the Cosa object may become inconsistent.

Please note that documentation for each device driver hardware module may be found in the Cosa/references directory. 

For now the target architecture is Arduino based on Atmega328P; Arduino Uno, Nano, etc. 

The target of this project is to provide an efficient programming platform for rapid prototyping of "Internet-of-things"-devices. Unfortunately Cosa is not a beginners entry level programming platform. 


