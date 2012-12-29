CosaNRFreceiver
==============

Demonstration of Nordic Semiconductor nRF24L01+ single chip 2.4GHz
transceiver driver. The receiver can wait for data from up to five
senders (CosaNRFsender). QoS data is sent. This is the simplest
usecase for this advanced low power transceiver.  

Circuit
-------
The SPI pins are used; 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK).
Chip activity enable RX/TX) is performed with pin 9, and interrupt
with pin 2. 

These pins are the wiring of the Nano IO shield.





