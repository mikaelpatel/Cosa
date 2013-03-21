Che Cosa?
=========

Cosa is an object-oriented platform for Arduino that supports an
event-driven programming paradigm with simple finite state
machines. It contains a rich set of classes to support rapid
prototyping of Internet-of-Things devices. Cosa supports the following
AVR/Atmega328P internal hardware modules; all pin modes, Digital,
Analog and Interrupt Pins, Analog Comparator, PWM, Watchdog, Timer0
(RTC), Timer1 (Servo), UART, SPI and TWI.  

Though object-oriented with optional operator overloading syntax
Cosa is between 2-10X faster than Arduino with regard to digital pin
functions. This comes with a small price-tag; memory, 4 bytes per
digital pin and 9 bytes per analog pin. Cosa analog pin objects
holds the latest sample and allows an event handler. See the
benchmarks in the examples directory for further details.

Cosa also contains a data streaming format (Ciao) for encoding of
C/C++ language data types such as strings, integer and floating
pointer numbers into a binary format. It may be used for a number of
applications; tracing, remote procedure calls, data exchange between
Arduino devices, etc. The format allows user data types to be defined
and values exchanged without additional encoding. The stream header
itself is a pre-defined serializable data type. Ciao is used to define
an Arduino monitoring and control language (Cosa fai) which has much
in common with Firmata. See
[CIAO.txt](https://github.com/mikaelpatel/Cosa/blob/master/CIAO.txt)
for more details and the example code (examples/Ciao). 

The primary programming paradigm is object-oriented and
state-machine/event driven with proto-threads. There is a large number
of device drivers available for SPI, TWI/I2C and 1-Wire. A strict
directory structure is used to organize the Cosa/driver source
code. Sub-directories are used for each driver type. This allows a
foundation for scaling and configuration.

Cosa uses the Arduino IDE and build system. Cosa classes are included
with prefix, e.g. "Cosa/FileName.hh". It is possible to use both
Arduino and Cosa functions together, though in some cases the Cosa
objects may become inconsistent. 

To improve debugging and testing there is trace/syslog style support. 
The IOStream class allows output to both serial communication
(UART/VWIO) and small TFT displays (such as the ST7735R and
PCD8544). The drawing Canvas class supports basic drawing operation
and scripting to reduce program memory footprint. The Canvas class
also supports drawing of icons and multiple fonts (GLCD and UTFT). 

The popular VirtualWire library has been refactored to the
object-oriented style of Cosa and extended with three additional
codecs; Manchester, 4B5B and Fixed Bitstuffing. This allows basic
ultra cheap wireless nodes with RF315/433 receiver and
transmitter. For more advanced wireless connections there is also a
driver for the Nordic Semiconductor NRF24L01+ chip, which allows
low-power wireless communication of up to 2 Mbps in the 2.4GHz band. 


The goal of this project is to provide an efficient programming
platform for rapid prototyping of "Internet-of-things"-devices. 
Unfortunately Cosa is not a beginners entry level programming
platform, though following some of the design patterns in Cosa will
help beginners build more complex small scale embedded systems with
richer concurrency and low power consumption.  

Please follow the development of this project on
[blogspot](http://cosa-arduino.blogspot.se/) and on the [Arduino
forum](http://arduino.cc/forum/index.php/topic,150299.0.html).  


Install
-------

To install download and move the Cosa directory to the Arduino cores
directory within arduino itself, i.e.,
your-arduino-install-directory/hardware/arduino/cores/arduino. The
examples should be moved to your Arduino sketchbook directory. 

The application programmers interface documentation is available as
HTML in the file doc.zip. Uncompress this file and navigate your
browser to the index file. The documentation contains a full
hyperlinked description of all functions in Cosa together with UML
graphs of the class hierarchy, include dependencies, and much more.

The provided documentation is generated with doxygen and may also be
generated for users source code if the Cosa documentation style is
adapted. See the Doxyfile for configuration of doxygen. 

For ATtiny a patch is needed to allow linking programs larger than 4K;
Please see
https://github.com/TCWORLD/ATTinyCore/tree/master/PCREL%20Patch%20for%20GCC 


Drivers
-------

1. DS18B20 Programmable Resolution 1-Wire Digital Thermometer.
2. AT24CXX Serial EEPROM.
3. DS1307 Realtime clock with RAM.
4. PCF8591 2-Wire 8-bit A/D and D/A converter.
5. ADXL345 Digital Accelerometer.
6. nRF24L01 Single Chip 2.4GHz Transceiver. 
7. DHT11 Humidity & Temperature Sensor.
8. HC-SR04/US-020 Ultrasonic range module.
9. ST7735R, 262K Color Single-Chip TFT Controller.
10. PCD8544 48x84 pixels matrix LCD controller/driver.
11. TSOP4838 IR Receiver Modules for Remote Control Systems.
12. Virtual Wire (VWI) on RF315/433 modules.
13. Slave device support for SPI, TWI and OWI.

The reference documentation for each device driver hardware module may
be found in the Cosa/references directory.  

References
----------

1. D.tools, http://hci.stanford.edu/research/dtools/
2. Processing, http://processing.org/
3. Wiring, http://wiring.org.co/
4. Arduino, http://www.arduino.cc/
5. Firmata, http://www.firmata.org/wiki/Main_Page
6. LilyPad, http://web.media.mit.edu/~leah/LilyPad/
7. Jeelabs, http://jeelabs.org/
8. Quantum Leaps, http://www.state-machine.com/
9. Concurrency.cc, http://concurrency.cc/
10. Protothreads, http://dunkels.com/adam/pt/
11. Arduino ATtiny, http://hlt.media.mit.edu/?p=1695
12. Arduino Low Power, http://gammon.com.au/power
13. Virtual Wire, http://www.open.com.au/mikem/arduino/VirtualWire/

Naming
------

* "Cosa"; noun thing, matter; pronoun; what?, what!. 
* "Che cosa"; pronoun; what. 
* "Ciao"; interjection hello!, goodbye!. 
* "Cosa fai"; what do you do?

Note
----

ATtiny, and Atmega328P, Atmega1284P, Atmega2560 based Arduino boards
(Uno, Mini, Nano, LilyPad, Mighty, Mega, etc) are supported. 
Arduino and AVR lib functions may be used together with Cosa. 

News
----

2013-01-23 Arduino Mega 2560 initial port.  
2013-01-25 UTFT font support added.  
2013-01-29 Proto-threads implemented. Draw/fill round rectangle added
to Canvas.  
2013-02-01 Adding doxygen generated documentation (doc.zip). 
GLCD font support added. IR remote receiver initial example.  
2013-02-05 Adding input/output operators to IOStream, Pins and
SPI. More detailed benchmarks and comparison with Arduino added.  
2013-02-07 IOStream driver for PCD8544 48x84 pixels matrix LCD
controller with support for fonts and icons.  
2013-02-10 TSOP4838 IR Receiver Modules support (LG remote
example). Improving PCD8544 driver with OffScreen canvas.  
2013-02-11 UART receiver, Device::getchar(), implemented. Adding
generic circlic buffer for IOStream::Device.  
2013-02-19 Adding support for ATtinyX5. Simple ATtiny monitor included.  
2013-02-19 Adding support for RC Servo.  
2013-02-23 Adding support for ATmega1284P/Mighty boards.  
2013-02-24 Refactoring and porting VirtualWire to Cosa.   
2013-03-03 Virtual Wire Interface (VWI) for ATtiny. VWI IOStream
Device to allow streaming of output over Virtual Wire.   
2013-03-05 Introducing Manchester Phase Encoder (MPE) based on VWI.  
2013-03-07 Introducing class for Power Management and Sleep Modes.   
2013-03-09 Refactoring VWI to allow easy update of codecs;
VirtualWire, Manchester and Fixed Bitstuffing.   
2013-03-11 Additional codex; Block Coding, B4B5.  
2013-03-17 New example for VWI with retransmission; simple client/server
example.   
2013-03-21 Introducing an interface for interrupt handlers, and
support for pin change interrupts. New directory structure for 
handling boards.  

