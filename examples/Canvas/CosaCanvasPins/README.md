CosaCanvasPins
==============

Cosa demonstration of Canvas device driver for ST7735R, 262K Color 
Single-Chip TFT Controller, and monitoring of Arduino pins.
Shows binding to IOStream::Device onto the display (and not the UART).

Circuit
-------
Using a HY-1.8 SPI TFT 128* 160 board connect as follows (Arduino ==>
HY-1.8 SPI):

GND ==> GND(1)  
VCC(5V) ==> VCC(2)  
RST ==> RESET(6)  
Pin(9) 	==> A0(7)  
MOSI/Pin(11) ==> SDA(8)  
SCK/Pin(13) ==> SCK(9)  
SS/Pin(10) ==> CS(10)  
VCC(5V) ==> LED+(15)  
GND ==> LED-(16)    




