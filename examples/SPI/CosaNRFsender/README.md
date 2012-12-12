CosaNRFsender
====

Demonstration of Nordic Semiconductor nRF24L01+ single chip 2.4GHz transceiver driver. The sender will transmit QoS data to the receiver. Modify the address "cosa1" to allow up to five senders to the same receiver. The sender address are "cosa1"..."cosa5". The receiver, CosaNRFreceiver, does not need to be modified. This demonstrates the MultiCeiver mode. 

Circuit

The SPI pins are used; 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK).
Chip activity enable RX/TX) is performed with pin 9, and interrupt with pin 2.

These pins are the wiring of the Nano IO shield.

