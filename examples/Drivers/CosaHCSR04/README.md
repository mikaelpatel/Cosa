CosaHCSR04
==========

Demonstration of Cosa HC-SR04 Ultrasonic range module device driver. 
Sample using the event manager and print distance. This example
supports display of the distance measurement on a TFT display
(ST7735R). 

Circuit
-------
For this example you need a HC-SR04 sensor/module. Connect HC-SR04
Trig Pin to Arduino Pin 2, and Echo Pin to Arduino Pin 3. Connect
power (VCC) and ground.  

The Arduino Pin 13 (builtin LED) is on during reading printout.

For display on TFT connect a ST7735R according to the driver default
pin configuration (10 (SS), 11 (MOSI), 13 (SCK) and 9 (CS), 10 (D/C),
RST). 

And enable in the sketch by defining USE_TFT.


