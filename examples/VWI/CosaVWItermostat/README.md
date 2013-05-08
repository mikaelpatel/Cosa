CosaVWItermostat
===============
Demonstration of the Virtual Wire Interface (VWI) driver and
simple logic to turn relays on and off depending on received
message with temperature and humidity reading from CosaTinyDHT11.
Turn on heater (relay#1) @ 22 C and off @ 26 C, and fan (relay#2)
@ 70 %RH and off @ 50 %RH. The fan logic will also turn on and
off a remote vent using the NEXA driver.

Circuit
-------
1. Connect RF433/315 Receiver to Arduino/ATtiny D8/D1 => RX DATA.
2. Relay modules to Arduino D2 => relay#1 (heater), 
   and D3 => relay#2 (fan).
3. RF433/315 Transmitter to D4.
4. UART module RX to D0 (ATtiny only).
