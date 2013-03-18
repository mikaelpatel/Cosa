CosaVWIsender
=============
Demonstration of the Virtual Wire Interface (VWI) driver.
Transmits a simple message with identity, message number,
and two data element; analog samples. Server should send back
an acknowledgement message with the identity and message number.
If an acknowledgement is not received a retransmission will occur.

Circuit
-------
Connect RF433/315 Transmitter Data to Arduino D9, RF433/315
Receiver to Arduino D8. Connect VCC and GND. Connect Arduino
analog pins A2 and A3 to analog sensors.  


