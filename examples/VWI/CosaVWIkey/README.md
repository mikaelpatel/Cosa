CosaVWIkey
==========
Demonstration of the Virtual Wire Interface (VWI) with VirtualWire
codec on ATtiny85. Uses the ExternalInterruptPin for wakeup after
power down. Run CosaVWIreceiver.ino on an Ardunio to receiver the
messages sent when releasing the button.

Circuit
-------
Connect RF433/315 Transmitter Data to ATtiny85 D1, connect VCC and
GND. Connect button with pullup resistor to D2.
