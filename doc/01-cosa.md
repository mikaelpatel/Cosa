# Che Cosa?

Cosa is an object-oriented platform for Arduino. It replaces the Arduino
and Wiring library with a large set of integrated classes that support
the full range of AVR/ATmega/ATtiny internal hardware modules; all pin
modes, Digital, and Analog Pins, External and Pin Change Interrupts,
Analog Comparator, PWM, Watchdog, Timer0 (RTC), Timer1 (Servo), UART, USI,
SPI, TWI and EEPROM.

Cosa is implemented as an Arduino IDE core. The Cosa platform can be
used with almost all Arduino boards and ATtiny/ATmega processors. All
classes may be compiled for all variants. The limitations are hardware
resources.

Though object-oriented with optional operator overloading syntax,
Cosa is between 2-10X faster than Arduino with regard to digital pin
functions. This comes with a small price-tag; memory, 4 bytes per
digital pin and 9 bytes per analog pin. Cosa analog pin objects
holds the latest sample and allows an event handler. See the
benchmarks in the examples directory for further details.

Cosa contains several data streaming formats for message passing and
data streaming. Google Protocol Buffers are supported together with a
data streaming format (Ciao) for encoding of C/C++ language data types
such as strings, integer and floating pointer numbers into a binary
format. It may be used for a number of applications; tracing, remote
procedure calls, data exchange between Arduino devices, etc. The
format allows user data types to be defined and values exchanged
without additional encoding.

The primary programming paradigm is object-oriented and state-machine,
event driven, with proto-threads or multi-tasking. There is a large
number of device drivers available for SPI, I2C (TWI) and 1-Wire
(OWI). A strict directory structure is used to organize the
Cosa/driver source code. Sub-directories are used for each driver
type. This allows a foundation for scaling and configuration.

Cosa uses the Arduino IDE and build system. Cosa classes are included
with prefix, e.g. "Cosa/FileName.hh". There is also (for Linux) an
advanced build system that allow make-based build and caching of core
library without writing makefiles. It also support the typical
development steps; compile, upload, and serial monitoring.

To improve debugging and testing there is assert/trace/syslog style
support. The IOStream class allows output to both serial wire/wireless
communication (UART/VWIO) and small TFT displays (such as the ST7735,
ST7565, HD44780, and PCD8544). The Cosa LCD class extends
IOStream::Device with additional common LCD functions. The Cosa LCD
Menu class adds a simple framework for creating menu systems with
program state such as integer ranges, bitsets and enumeration
variables. All menu data structures are stored in program memory and
the SRAM requirement is minimum. A macro set hides the details of
creating the data structures in program memory.

The drawing Canvas class supports basic drawing operation
and scripting to reduce program memory footprint. The Canvas class
also supports drawing of icons and a large set of fonts.

The popular VirtualWire library has been refactored to the
object-oriented style of Cosa (VWI) and extended with three additional
codecs; Manchester, 4B5B and Bitstuffing. This allows basic ultra
cheap wireless nodes with RF315/433 receiver and transmitter. For more
advanced wireless connections there is also a driver for the Nordic
Semiconductor NRF24L01+ chip, which allows low-power wireless
communication of up to 2 Mbps in the 2.4GHz band, and the TI CC1101
Low-Power Sub-1 GHz RF Transceiver.

The goal of this project is to provide an efficient programming
platform for rapid prototyping of "Internet-of-things"-devices. There
is an Ethernet/Socket with W5100 Ethernet controller device
driver. This implementation allows streaming direct to the device
buffers. Cosa also implements a number of IP protocols; DNS, DHCP,
NTP, HTTP, and SNMP, and high level messaging such as MQTT and
ThingSpeak. There is also support for the TI CC3000 WiFi module.

Unfortunately Cosa is not a beginners entry level programming
platform, though following some of the design patterns in Cosa will
help beginners build more complex small scale embedded systems with
richer concurrency and low power consumption.