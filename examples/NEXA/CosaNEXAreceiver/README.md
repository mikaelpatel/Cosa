CosaNEXAreceiver
===============

Simple sketch to demonstrate receiving Nexa Home Wireless Switch
Remote codes. First command received will be used as the device 
identity. Sucessive commands are compared against the device identity
and if matches the built-in LED is set on/off according to the
command. Commands may be transmitted with a NEXA Remote or with
a RF433 Transmitter and CosaNEXAsenser sketch.

Circuit
-------
For this example you need a NEXA Wireless Remote Controller and a
RF433 Receiver. Connect Arduino Pin EXT0 (D2) to receiver data
pin. Connect power (VCC) and ground. 

On an ATtinyX5 please connect a LED to pin D4 in serie with a resistor
(approx 300-500 ohm) to ground.


