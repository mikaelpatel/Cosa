CosaTemp
=================

Demonstration of temperature reading from a DS18B20 and presentation
on a PCD8544 device together with off-screen bitmap drawing.

Circuit
-------
The PCD8544 should be connect using 3.3 V signals and VCC. Back-light 
should be max 3.3 V. Reduce voltage with 100-500 ohm resistor to ground.

The default pins are used (Arduino => PCD8544):

    D6 ==> SDIN  
    D7 ==> SCLK  
    D8 ==> DC  
    D9 ==> SCE  
    RST ==> RST  

The 1-wire temperature sensor DS18B20 to D5.