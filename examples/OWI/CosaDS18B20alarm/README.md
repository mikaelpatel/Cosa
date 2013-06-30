CosaDS18B20alarm
================

Demonstration of Cosa DS18B20 1-Wire driver alarm handling. 

Circuit
-------
The Arduino Pin 7 is used as the 1-wire bus. Use a pullup resistor
(4-5 Kohm) to power (VCC). Connect VCC and ground to the Digital
Thermometer. The example code assumes three devices on the 1-wire bus
but this is not critical. 

The Arduino Pin 13 (builtin LED) is on during the temperature
sampling. 

