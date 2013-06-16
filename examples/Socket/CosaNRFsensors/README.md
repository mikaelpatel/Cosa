CosaNRFsensors
==============

Cosa demonstration of NRF24L01+ driver; read and send sensor
data to monitor (CosaNRFmonitor) using sockets. Supports reading of
analog pins, DHT11 and 1-Wire DS18B20 sensors.

Circuit
-------
The SPI pins are used; D10 (CSN), D11 (MOSI), D12 (MISO), D13 (SCK).
Chip activity enable RX/TX is performed with pin D9 (CE), and
interrupt with pin D2 (IRQ). The pins are wired as on the Nano IO
shield. 

Luminance and temperature sensor based on analog pins(A2, A3). Digital
humidity and temperture sensor on pin(D7). 1-Wire bus with temperature
sensor DS18B20 on pin(D8).



