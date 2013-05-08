CosaTermostat
=============

Demonstration of temperature and humidity reading from a DHT11 and
control of heater and fan with output pins.

Circuit
-------
The DHT11 sensor to Arduino D7/ATtiny D1. The relay for heater control
on D6/D2 and fan control on D5/D3 (Arduino/ATtiny ==> Device).

    --/D0 ==> UART RX
    D7/D1 ==> DHT11 data  
    D6/D2 ==> Relay/heater  
    D5/D3 ==> Relay/fan  
