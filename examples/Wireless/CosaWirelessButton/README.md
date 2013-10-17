CosaWirelessButton
==================
Demonstration of the Wireless interface and device drivers. Uses the
ExternalInterruptPin for wakeup after power down. Run
CosaWirelessDebug.ino or CosaWirelessReceiver.ino to receiver the
messages sent when the button is pushed. 

Circuit
-------
Connect RF433/315 Transmitter Data to Arduino/ATtiny D9/D1, 
connect VCC and GND. Connect button with pullup resistor to 
Arduino EXT1/D3, Mega EXT2/D19, Mighty/D10, TinyX4/EXT0/D10, and 
TinyX5/EXT0/D2.

