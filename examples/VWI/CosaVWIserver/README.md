CosaVWIserver
=============

Demonstration of the Virtual Wire Interface (VWI) driver.
Receive and print a simple message with identity, message number,
and 2x16-bit analog data sample. Send an acknowledge by sending 
a message with the received identity and message number.

Circuit
-------
Connect RF433/315 Transmitter Data to Arduino D12, RF433/315
Receiver to Arduino D11. Connect VCC and GND. 
