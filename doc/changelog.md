Cosa's changelog
================

2014-03 Adding message passing with Actors and additional benchmarks
for Nucleo. Support for ThingSpeak client (channel update) and
TalkBack command handling. Improving the ADXL345 accelerometer device
driver and example sketch. Adding a Soft SPI implementation (only 0/2
mode). Refactoring directories for example sketches.       

2014-02 Adding support for coroutines (threads), semaphores and
mutex; Nucleo. Allow build with GCC 4.8.1. Support for MQTT Client,
HTTP Server and Client implementation. Additional SNMP agent
implementation. RTC::Timer for micro-second level time
events. Improved Time handling with integration with RTC and NTP to
allow sync.          

2014-01 Adding support for Arduino IDE 1.5.X. Including a Wireless
interface implementation for RFM69W/HW. Socket interface and device
driver for W5100 Ethernet Controller. INET network address handler
support. DHCP, DNS and NTP client/request handler. Simple Telnet port
handler for trace output.       

2013-12 DS1302 RTC and SD card support. Simple FAT16 library using
SD. Support for RS485 with master-slave protocol. ATmega32u4 support
added with USB/CDC.      

2013-11 New device driver for MPU6050 Motion Processing Unit; Digital
thermometer, accelerometer and gyroscope. Adding support for bitsets
and Google Protocol Buffers data encoding/decoding. Character and
token scanner to IOStream. Allowing blocking and non-blocking IOStream
device mode. Added support for ATtinyX61 and a variant of Base64 for
encoding of binary data.    

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

2013-09 Major refactoring of SPI and TWI device driver support.    

2013-08 Updating DHT11/22 device driver. Adding a simple touch
capacitive sensor.     

2013-07 Introducing an abstract LCD::Device class and refactoring LCD
device drivers. Benchmarking and optimizing LCD device drivers. Added
an event driven resistor net keypad handler and support for the
DFRobot LCD Keypad shield. Boosting LCD performance to 2-6X faster
than Arduino library. Added a new LCD menu system. Refactored Cosa
directory structure to match the Arduino core file
structure. Performance tuning LCD adapters; additional adapters with
shift register support (SR3W and SR4W). Adding Vigenere autokey and
RC4 cipher.       

2013-06 HD44780 (1602) LCD device driver. Adding support for
Sockets. Refactoring NRF24L01P device driver to a
Socker::Device with support for both connection-less and
connection-oriented communication. DS3231 device driver and example
sketches.  Driver for the PCF8574/PCF8574A Remote 8-bit I/O expander
for I2C-bus with interrupt. Support for 20x4 LCD.    

2013-05 Improving ATtiny support. Adding driver for DHT22. NEXA
Wireless Remote command transmitter (RF433). Introducing a new
template class for handling of keyed sets of event handlers. Added
support for LCD ST7565 with natural text scrolling. Making it easy to
implement TWI slave devices with TWI::Device. Including support for
event driven Rotary Encoder handler.   

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
=======