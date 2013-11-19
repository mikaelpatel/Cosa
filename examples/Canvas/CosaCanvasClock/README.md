CosaCanvasClock
===============

Demonstration of the Cosa Canvas/UTF Font manager on the ST7735, 262K Color
Single-Chip TFT Controller device driver; A simple count down clock.

Circuit
-------
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
