Che Cosa?
---------

Cosa is an object-oriented platform for Arduino. It replaces the Arduino
and Wiring library with a large set of integrated classes that support
the full range of AVR/ATmega/ATtiny internal hardware modules; all pin
modes, Digital, and Analog Pins, External and Pin Change Interrupts,
Analog Comparator, PWM, Watchdog, Timer0/Timer2 (RTT), Timer1
(Servo/Tone/VWI), Input Capture, UART, USI, SPI, TWI and EEPROM. Cosa
supports several programming paradigms including Multi-Tasking, Event
Driven Programming and UML Capsules/Actors. Cosa contains over 200
classes and nearly as many example sketches to get started.

Please note that Cosa is not an Arduino core; Cosa does not implement
the Arduino API. Sketches written with Cosa may be built with the
Arduino IDE or with the command line based build support for Linux.

Cosa supports the Arduino Boards Manager install. Simply install by
adding the Cosa Package URL to the Additional Boards Manager URLs in
the IDE Preferences; https://raw.githubusercontent.com/mikaelpatel/Cosa/master/package_cosa_index.json


More details are available:

* [Che Cosa?](./doc/01-cosa.md)
* [How to install Cosa](./doc/02-install.md)
* [Built-in drivers](./doc/04-drivers.md)
* [External Libraries, Tools and Applications](./doc/05-libraries.md)

Cosa API
--------

The API documentation is available:

* [online](https://mikaelpatel.github.io/Cosa/index.html)
* Compressed for [download](https://github.com/mikaelpatel/Cosa/releases)

The documentation contains a full hyperlinked description of all functions
in Cosa together with UML graphs of the class hierarchy, include dependencies,
and much more.

The provided documentation is generated with doxygen and may also be
generated for users source code if the Cosa documentation style is
adapted. See the Doxyfile for configuration of doxygen.

Supported Boards and MCUs
-------------------------

Cosa has built-in support for a large number of boards and
clones/vendors. It also supports breadboards and custom-design
boards with AVR MCUs.

* [AdaFruit ATmega32U4](https://github.com/mikaelpatel/Cosa-AdaFruit/blob/master/variants/adafruit/atmega32u4/Board.hh)
* [Anarduino MiniWireless](https://github.com/mikaelpatel/Cosa-Anarduino/blob/master/variants/anarduino/miniwireless/Board.hh)
* [Arduino Diecimila](./cores/cosa/Cosa/Board/Arduino/Diecimila.hh)
* [Arduino Duemilanove](./cores/cosa/Cosa/Board/Arduino/Duemilanove.hh)
* [Arduino Leonardo](./cores/cosa/Cosa/Board/Arduino/Leonardo.hh)
* [Arduino Mega 1280](./cores/cosa/Cosa/Board/Arduino/Mega.hh)
* [Arduino Mega 2560](./cores/cosa/Cosa/Board/Arduino/Mega.hh)
* [Arduino Micro](./cores/cosa/Cosa/Board/Arduino/Micro.hh)
* [Arduino Nano](./cores/cosa/Cosa/Board/Arduino/Nano.hh)
* [Arduino Pro Micro](./cores/cosa/Cosa/Board/Arduino/Pro_Micro.hh)
* [Arduino Pro Mini](./cores/cosa/Cosa/Board/Arduino/Pro_Mini.hh)
* [Arduino Uno](./cores/cosa/Cosa/Board/Arduino/Uno.hh)
* Breadboard ([ATtinyX4](./cores/cosa/Cosa/Board/Arduino/ATtinyX4.hh), [ATtinyX5](./cores/cosa/Cosa/Board/Arduino/ATtinyX5.hh), [ATtinyX61](./cores/cosa/Cosa/Board/Arduino/ATtinyX61.hh), [ATmega328](./cores/cosa/Cosa/Board/Arduino/ATmega328P.hh), [ATmega1284](./cores/cosa/Cosa/Board/Arduino/ATmega1284P.hh))
* [ITEAD Studio IBoard](https://github.com/mikaelpatel/Cosa-ITEADStudio/blob/master/variants/iteadstudio/iboard/Board.hh)
* [LilyPad Arduino](./cores/cosa/Cosa/Board/Arduino/LilyPad.hh)
* [LilyPad Arduino USB](./cores/cosa/Cosa/Board/Arduino/LilyPad_USB.hh)
* [LowPowerLab Moteino](https://github.com/mikaelpatel/Cosa-LowPowerLab/blob/master/variants/lowpowerlab/moteino/Board.hh)
* [LowPowerLab Moteino Mega](https://github.com/mikaelpatel/Cosa-LowPowerLab/blob/master/variants/lowpowerlab/moteino-mega/Board.hh)
* [Pinoccio Scout](https://github.com/mikaelpatel/Cosa-Pinoccio/blob/master/variants/pinoccio/scout/Board.hh)
* [Microduino-Core](https://github.com/mikaelpatel/Cosa-Microduino/blob/master/variants/microduino/core/Board.hh)
* [Microduino-Core32u4](https://github.com/mikaelpatel/Cosa-Microduino/blob/master/variants/microduino/core32u4/Board.hh)
* [Microduino-Core+](https://github.com/mikaelpatel/Cosa-Microduino/blob/master/variants/microduino/core-plus/Board.hh)
* [PJRC Teensy 2.0](https://github.com/mikaelpatel/Cosa-PJRC/blob/master/variants/pjrc/teensy-2.0/Board.hh)
* [PJRC Teensy++ 2.0](https://github.com/mikaelpatel/Cosa-PJRC/blob/master/variants/pjrc/teensypp-2.0/Board.hh)
* [Wicked Device WildFire V3](https://github.com/mikaelpatel/Cosa-WickedDevice/blob/master/variants/wickeddevice/wildfire/Board.hh)
* [Custom Boards](https://github.com/mikaelpatel/Cosa-CustomBoard)

References
----------

1. D.tools, http://hci.stanford.edu/research/dtools/
2. Processing, http://processing.org/
3. Wiring, http://wiring.org.co/
4. Quantum Leaps, http://www.state-machine.com/
5. Concurrency.cc, http://concurrency.cc/
6. Arduino, http://www.arduino.cc/
7. Firmata, http://www.firmata.org/wiki/Main_Page
8. LilyPad, http://web.media.mit.edu/~leah/LilyPad/
9. Adafruit, http://www.adafruit.com/
10. Microduino, http://www.microduino.cc/
11. Jeelabs, http://jeelabs.org/
12. Teensy, https://www.pjrc.com/
13. Pinoccio, https://pinocc.io/
14. LowPowerLab, http://lowpowerlab.com/
15. Anarduino, http://www.anarduino.com/
16. Wicked Device, http://shop.wickeddevice.com/
17. MQTT, http://mqtt.org/
18. ThingSpeak, https://thingspeak.com/

Naming
------

* "Ciao"; interjection hello!, goodbye!.
* "Che cosa"; pronoun; what.
* "Cosa"; noun thing, matter; pronoun; what?, what!.
* "Cosa fai"; what do you do?
* "Nucleo"; kernel.
* "Rifare"; remake.
* "Rete"; network.
* "Tutto"; all.

Stay tuned
----------

Please follow the development of this project on the blog
http://cosa-arduino.blogspot.se and on the Arduino forum,
http://arduino.cc/forum/index.php/topic,150299.0.html.

Supporting this project
-----------------------

There are many ways to support this project.

1. Build and test.
2. Fix bugs and take part in the development work.
3. Write blog or instructions on how you have used Cosa in your
project(s).
4. Donate hardware; Arduino boards and shields. This is especially
welcome as with the growning number of device drivers and supported
modules that require hardware setups which takes a lot of time if
breadboarded.
5. Or donations through paypal (use email address).

If you are planning to use Cosa you are encouraged to support the
project to help keep the software at high quality and follow
changes in tooling, and above all open-source.

Please note that the issues list should be used mainly for bug reports
and enhancement requests. Design and implementation support is only
provided to contributing projects.
