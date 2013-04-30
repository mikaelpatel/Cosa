CosaTinyPCD8544
===============

Demonstration of the PCD8544 device driver with mapping to
IOStream::Device for the TINYX5.

Circuit
-------
The PCD8544 should be connect using 3.3 V signals and VCC. Back-light 
should be max 3.3 V. Reduce voltage with 100-500 ohm resistor to ground.

Connect ATtinyX5 to PCD8544 (ATtinyX5 => PCD8544):
   D0 ==> SDIN
   D1 ==> SCLK
   D2 ==> DC
   D3 ==> SCE

Connect VCC and GND to PCD8544.


