CosaMPEsender
=============
Demonstration of the Manchester Phase Encoder (MPE) driver. Transmits a
simple message with identity, message number, and 2x16-bit analog data
elements. See CosaMPEreceiver.

Circuit
-------
Connect RF433/315 Transmitter Data to Arduino(ATtiny85) D12(D2),
VCC to Arduino D10(D1) and connect GND. Connect Arduino analog pins
A2 and A3 to analog sensors. On ATtiny85 the pins are D2, D1. 
The power control pin D10(D1) can also be used for a LED.




