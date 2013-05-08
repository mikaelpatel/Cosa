CosaDS18B20
===========

Demonstration of Cosa OneWire DS18B20 1-Wire driver. Three
Programmable Resolution 1-Wire Digital Thermometers are used. 

Circuit
-------
The Arduino Pin 7 is used as the 1-wire bus. Use a pullup resistor
(4-5 Kohm) to power (VCC). Connect VCC and ground to the Digital
Thermometer. The example code assumes three devices on the 1-wire bus
but this is not critical. Will not work in parasite power mode.

The Arduino Pin 13 (builtin LED) is on during the temperature
sampling. 

