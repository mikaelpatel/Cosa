CosaVWIkey
==========
Demonstration of the Virtual Wire Interface (VWI) with VirtualWire
codec on ATtiny85. Uses the ExternalInterruptPin for wakeup after
power down. Run CosaVWIreceiver.ino on an Ardunio to receiver the
messages sent when releasing the button.

Circuit
-------
Connect RF433/315 Transmitter Data to Arduino/ATtiny D9/D1, 
connect VCC and GND. Connect button with pullup resistor to 
Arduino EXT0/D2, Mega EXT2/D19, Mighty/D10, TinyX4/D10, and 
TinyX5/D2.

