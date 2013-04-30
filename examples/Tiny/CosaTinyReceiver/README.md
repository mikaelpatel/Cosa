CosaTinyReceiver
================

Demonstration of the PCD8544 device driver with mapping to
IOStream::Device for the TINYX5 and wireless communication using
RF433. Receives messages from CosaVWIsender and display on LCD/PCD8544.

Circuit
-------
The PCD8544 should be connect using 3.3 V signals and VCC. Back-light 
should be max 3.3 V. Reduce voltage with 100-500 ohm resistor to ground.

Connect ATtinyX5 to PCD8544 (ATtinyX5 => PCD8544):
   D0 ==> SDIN
   D1 ==> SCLK
   D2 ==> DC
   D3 ==> SCE

Connect ATtinyX5 to RF433 Receiver: 
   D4 <== DATA

Connect VCC and GND to PCD8544 and RF433.
