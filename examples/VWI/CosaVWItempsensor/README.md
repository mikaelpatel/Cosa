CosaVWItempsensor
=================
Demonstration sending temperature readings from a OneWire DS18B20
device over the Virtual Wire Interface (VWI). 

This sketch is designed for an ATtiny85 running on the internal 
8 MHz clock. Receive measurements with the CosaVWItempmonitor sketch.

Circuit
-------
Connect RF433/315 Transmitter Data to ATtiny85 D1, connect VCC 
GND. Connect one or two 1-Wire digital thermometer to D2 with pullup
resistor (approx. 4.7 Kohm), connect VCC and GND. In parasite mode connect
thermometer VCC pin also to GND.



