CosaTinyTermostat
=================

Demonstrate soft uart for the ATtiny. Trace output from ATtiny. This
is an ATtiny version of the termostat demo (CosaTermostat). 

Circuit
-------
Connect DHT11 sensor to ATtiny D1, LED or relay to D2, and D3.
ATtiny D0 is trace output (soft uart) and should be connected
to an Arduino running the CosaTinyMonitor (ATtiny ==> Device).

   D0 ==> Arduino/D7
   D1 ==> DHT11 data  
   D2 ==> Relay/heater  
   D3 ==> Relay/fan  

