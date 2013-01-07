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

Using a HY-1.8 SPI TFT 128* 160 board connect as follows (Arduino ==>
HY-1.8 SPI):

GND ==> GND(1)  
VCC(5V) ==> VCC(2)  
RST ==> HY-18 RESET(6)  
Pin(9) 	==> A0(7)  
MOSI/Pin(11) ==> SDA(8)  
SCK/Pin(13) ==> SCK(9)  
SS/Pin(10) ==> CS(10)  
VCC(5V) ==> LED+(15)  
GND ==> LED-(16)    

And enable in the sketch by defining USE_TFT.


