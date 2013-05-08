CosaNEXAreceiver
===============

Simple sketch to demonstrate receiving Nexa Home Wireless Switch
Remote codes. First command received will be used as the device 
identity. Sucessive commands are dispatched through the Listener.
The built-in LED is set on/off according to the command when the
address of the command matches.. See Also CosaNEXAsender if you 
wish to run the sketch without a NEXA remote control.

Circuit
-------
For this example you need a NEXA Wireless Remote Controller and a
RF433 Receiver. Connect Arduino Pin EXT0 (D2) to receiver data
pin. Connect power (VCC) and ground. 

On an ATtinyX5 please connect a LED to pin D4 in serie with a resistor
(approx 300-500 ohm) to ground.
