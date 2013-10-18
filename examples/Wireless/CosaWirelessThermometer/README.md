CosaWirelessThermometer
=======================
Demonstration sending temperature readings from 1-Wire DS18B20
devices over the Wireless Interface and devices. Uses power down mode
with only watchdog active (5-6 uA on ATtiny85). This sketch is
designed to run on an ATtiny85 running on the internal 8 MHz clock. 

Circuit
-------
Connect RF433/315 Transmitter Data to ATtiny85 D0, connect VCC 
GND. Connect 1-Wire digital thermometer to D3 with pullup resistor.
The pullup resistor (4K7) may be connected to D4 to allow power
control. This sketch supports parasite powered DS18B20 devices.
Connect the DS18B20 VCC to GND. 


