CosaTermostat
=============

Demonstration of temperature and humidity reading from a DHT11 and
control of heater and fan with output pins.

Circuit
-------
The DHT11 sensor to Arduino D2/ATtiny85 D2/ATtiny84 D10. The relay 
for heater control on D6/D1 and fan control on D5/D3 (Arduino/ATtiny 
==> Device).

    --/D0 ==> UART RX
    D2/D10/D2 ==> DHT11 data  
    D6/D1 ==> Relay/heater  
    D5/D3 ==> Relay/fan  
