CosaCanvasHCSR04
================

Cosa demonstration of the Ultrasonic range module HC-SR04 driver.
Display splash screen with Arduino icon and range in milli-meters.

Circuit
-------
For this example you need a HC-SR04 sensor/module. Connect HC-SR04
Trig Pin to Arduino Pin 2, and Echo Pin to Arduino Pin 3. Connect
power (VCC) and ground.  

The Arduino Pin 13 (builtin LED) is used as a heart-beat for the
events. Using a HY-1.8 SPI TFT 128*160 board connect as follows:

Using a HY-1.8 SPI TFT 128*160 board connect as follows (Arduino ==>
HY-1.8 SPI):   

GND      ==>   GND(1),   
VCC(5V)  ==>   VCC(2),   
RST      ==>   RESET(6),  
D9       ==>   A0(7),   
MOSI/D11 ==>   SDA(8),   
SCK/D13  ==>   SCK(9),  
SS/D10   ==>   CS(10),   
VCC(5V)  ==>   LED+(15),   
GND      ==>   LED-(16)    

![HY-1.8 SPI Board](http://img04.taobaocdn.com/imgextra/i4/39773402/T2dYOmXflaXXXXXXXX_!!39773402.jpg)
