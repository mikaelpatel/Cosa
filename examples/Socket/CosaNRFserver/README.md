CosaNRFserver
=============

Cosa demonstration of NRF24L01+ driver; client-server socket
connect and send of messages. Run CosaNRFclient on another
Arduino.

Circuit
-------
The SPI pins are used; D10 (CSN), D11 (MOSI), D12 (MISO), D13 (SCK).
Chip activity enable RX/TX is performed with pin D9 (CE), and
interrupt with pin D2 (IRQ). The pins are wired as on the Nano IO
shield. 





