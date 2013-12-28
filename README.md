Che Cosa?
=========

Cosa is an object-oriented platform for Arduino that supports an
event-driven programming paradigm with simple finite state
machines. It contains a rich set of classes to support rapid
prototyping of Internet-of-Things devices. Cosa supports the following
AVR/ATmega/ATtiny internal hardware modules; all pin modes, Digital,
and Analog Pins, External and Pin Change Interrupts, Analog
Comparator, PWM, Watchdog, Timer0 (RTC), Timer1 (Servo), UART, USI,
SPI, TWI and EEPROM.   

Though object-oriented with optional operator overloading syntax
Cosa is between 2-10X faster than Arduino with regard to digital pin
functions. This comes with a small price-tag; memory, 4 bytes per
digital pin and 12 bytes per analog pin. Cosa analog pin objects
holds the latest sample and allows an event handler. See the
benchmarks in the examples directory for further details.

Cosa contains several data streaming formats for message passing and
data streaming. Google Protocol Buffers are supported together with a
data streaming format (Ciao) for encoding of C/C++ language data types
such as strings, integer and floating pointer numbers into a binary
format. It may be used for a number of applications; tracing, remote
procedure calls, data exchange between Arduino devices, etc. The
format allows user data types to be defined and values exchanged
without additional encoding. The stream header itself is a pre-defined
serializable data type. Ciao is used to define an Arduino monitoring
and control language (Cosa fai) which has much in common with
Firmata. See
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
(UART/VWIO) and small TFT displays (such as the ST7735, ST7564,
HD44780 and PCD8544). The Cosa LCD class extends IOStream::Device with
additional common LCD functions. The drawing Canvas class supports
basic drawing operation and scripting to reduce program memory
footprint. The Canvas class also supports drawing of icons and
multiple fonts (GLCD and UTFT).  

The popular VirtualWire library has been refactored to the
object-oriented style of Cosa (VWI) and extended with three additional
codecs; Manchester, 4B5B and Bitstuffing. This allows basic ultra
cheap wireless nodes with RF315/433 receiver and transmitter. For more
advanced wireless connections there is also a driver for the Nordic
Semiconductor NRF24L01+ chip, which allows low-power wireless
communication of up to 2 Mbps in the 2.4GHz band, and the TI CC1101
Low-Power Sub-1 GHz RF Transceiver. These are interchangable through
an abstract Wireless interface.

The primary goal of this project is to provide an efficient programming
platform for rapid prototyping of "Internet-of-things"-devices and
sensor networks. Unfortunately Cosa is not a beginners entry level
programming platform, though following some of the design patterns in
Cosa will help beginners build more complex small scale embedded
systems with richer concurrency and low power consumption.  

The projects main principle of evolution is iterative with many
rewrites and refactoring. With that said this phase of the project
will require users to align with the interface changes. When
interfaces become stable the project will switch to a normal release
style.

Please follow the development of this project on
[blogspot](http://cosa-arduino.blogspot.se/) and on the [Arduino
forum](http://arduino.cc/forum/index.php/topic,150299.0.html).  

Install
-------

The [Cosa zip file]
(https://github.com/mikaelpatel/Cosa/archive/master.zip) is an Arduino
core package. Download and unzip in your Sketchbook hardware
folder. Create the folder if missing. Restart the Arduino IDE and Cosa
will show up as a number of boards and example sketches.  

For ATtiny a patch is needed for Arduino Windows version to allow
linking programs larger than 4K; See
https://github.com/TCWORLD/ATTinyCore/tree/master/PCREL%20Patch%20for%20GCC. 
Do not forget to program the ATtiny device with the bootloader, i.e.,
set the fuse bits, before using the device for the first time.  

The Application Programmers Interface (API) documentation is available 
[online](http://dl.dropbox.com/u/993383/Cosa/doc/html/index.html) and
compressed for
[download](http://dl.dropbox.com/u/993383/Cosa/doc.zip). The
documentation contains a full hyperlinked description of all functions
in Cosa together with UML graphs of the class hierarchy, include
dependencies, and much more. 

The provided documentation is generated with doxygen and may also be
generated for users source code if the Cosa documentation style is
adapted. See the Doxyfile for configuration of doxygen. 

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
9. ST7735, 262K Color Single-Chip TFT Controller.
10. PCD8544 48x84 pixels matrix LCD controller/driver.
11. TSOP4838 IR Receiver Modules for Remote Control Systems.
12. Virtual Wire (VWI) on RF315/433 modules.
13. HMC5883L 3-Axis Digital Compass IC.
14. NEXA/HomeEasy Wireless Remote command transmitter/receiver for RF433.
15. ST7565, 65x132 Dot Matrix LCD Controller/Driver.
16. Touch capacitive sensor, debounced button and keypad.
17. Dials with Rotary Encoder.
18. HD44780 (aka 1602, 2004) LCD Controller/Driver.
19. DS3231, Extremely Accurate I2C-Integrated RTC/TCXO/Crystal. 
20. PCF8574/PCF8574A Remote 8-bit I/O expander for I2C-bus with interrupt. 
21. BMP085 Digital Pressure Sensor.
22. TI CC1101 Low-Power Sub-1 GHz RF Transceiver.
23. L3G4200D Digital Gryposcope.
24. MPU6050 Motion Processing Unit; Digital thermometer, accelerometer
and gyroscope.
25. DS1302 Tickle-Charge Timekeeping Chip.
26. SD card, SPI driver.
27. RS485 support; master-slave protocol.
28. Slave device support for SPI, TWI and OWI.

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
* "Rete"; network

Note
----

ATtinyX4/X5/X61, Atmega328P, Atmega1284P, Atmega2560 and Atmega32u4
based Arduino boards (Uno, Mini, Mini Pro, Micro, Nano, Leonardo,
LilyPad, LilyPad USB, Mighty, Mega, etc) are supported. ATmega32U4 
internal USB hardware (e.g. CDC) is not currently supported.

News
----

2013-01 Arduino Mega 2560 initial port. UTFT font support
added. Proto-threads implemented. Draw/fill round rectangle added to
Canvas.    
2013-02 Adding doxygen generated documentation (doc.zip). GLCD font
support added. IR remote receiver initial example. Adding input/output
operators to IOStream, Pins and SPI. More detailed benchmarks and
comparison with Arduino added. IOStream driver for PCD8544 48x84
pixels matrix LCD controller with support for fonts and icons.
TSOP4838 IR Receiver Modules support (LG remote example). Improving
PCD8544 driver with OffScreen canvas. UART receiver,
Device::getchar(), implemented. Adding generic circlic buffer for
IOStream::Device. Adding support for ATtinyX5. Simple ATtiny monitor
included. Adding support for RC Servo. Adding support for
ATmega1284P/Mighty boards. Refactoring and porting VirtualWire to
Cosa.    
2013-03 Virtual Wire Interface (VWI) for ATtiny. VWI IOStream Device
to allow streaming of output over Virtual Wire. Introducing Manchester
Phase Encoder (MPE) based on VWI. Introducing class for Power
Management and Sleep Modes. Refactoring VWI to allow easy update of
codecs; VirtualWire, Manchester and Fixed Bitstuffing. Additional
codex; Block Coding, B4B5. New VWI example sketches with
retransmission; simple client/server example. Introducing an interface
for interrupt handlers, and support for pin change interrupts. New
directory structure for handling boards. Moving documentation and
references to dropbox. Reducing size of download from github.   
2013-04 Adding support for HMC5883L 3-axis digital compass. Adding
abstraction of EEPROM with default handling of internal
EEPROM. Refactoring AT24CXX driver for new EEPROM
interface. Implemented 1-wire parasite power mode. Improved DS18B20
1-wire driver and demo sketches with VWI integration. Added support
for battery voltage monitoring. New extended mode in VWI with node
address, sub-net address matching, message sequence numbering and type
handling. NEXA Wireless Remote command receiver (RF433). Added a
reliable message passing protocol to the Virtual Wire Interface
(VWI). The new class VWI::Transceiver supports message acknowledgement
and auto-retransmission. Full support for message parsing.    
2013-05 Improving ATtiny support. Adding driver for DHT22. NEXA
Wireless Remote command transmitter (RF433). Introducing a new
template class for handling of keyed sets of event handlers. Added
support for LCD ST7565 with natural text scrolling. Making it easy to
implement TWI slave devices with TWI::Device. Including support for
event driven Rotary Encoder handler.   
2013-06 HD44780 (1602) LCD device driver. Adding support for
Sockets. Refactoring NRF24L01P device driver to a
Socker::Device with support for both connection-less and
connection-oriented communication. DS3231 device driver and example
sketches.  Driver for the PCF8574/PCF8574A Remote 8-bit I/O expander
for I2C-bus with interrupt. Support for 20x4 LCD.    
2013-07 Introducing an abstract LCD::Device class and refactoring LCD
device drivers. Benchmarking and optimizing LCD device drivers. Added
an event driven resistor net keypad handler and support for the
DFRobot LCD Keypad shield. Boosting LCD performance to 2-6X faster
than Arduino library. Added a new LCD menu system. Refactored Cosa
directory structure to match the Arduino core file
structure. Performance tuning LCD adapters; additional adapters with
shift register support (SR3W and SR4W). Adding Vigenere autokey and
RC4 cipher.       
2013-08 Updating DHT11/22 device driver. Adding a simple touch
capacitive sensor.     
2013-09 Major refactoring of SPI and TWI device driver support.    
2013-10 Device driver for CC1101 and BMP085 introduced. Introducting
an abstract Wireless device interface. Refactoring Virtual Wire,
CC1101 and NRF24L01P to the new interface. Improving support for low
power mode. Adding a Registry for mapping from path (index sequence)
to data storage in PROGMEM, EEPROM and SRAM. Mapping may also be to
Action object in SRAM. Support for LCD TWI port expander
GY-IICLCD. Wireless IOStream class introduced to allow binding of
Wireless device driver to IOStream and trace output over
Wireless. First draft of RETE; data distribution and network
management protocol. Adding support for L3G4200D digital gyroscope.    
2013-11 New device driver for MPU6050 Motion Processing Unit; Digital
thermometer, accelerometer and gyroscope. Adding support for bitsets
and Google Protocol Buffers data encoding/decoding. Character and
token scanner to IOStream. Allowing blocking and non-blocking IOStream
device mode. Added support for ATtinyX61 and a variant of Base64 for
encoding of binary data.    
2013-12 DS1302 RTC and SD card support. Simple FAT16 library using
SD. Support for RS485 with master-slave protocol. ATmega32u4 support
added.      


