CosaHD44780
===========
Demonstration of the HD44780 (aka 1602) LCD device driver with mapping 
to IOStream::Device.

Circuit
-------
The default pins are used (Arduino => PCD8544):

    D4 ==> D4
    D5 ==> D5
    D6 ==> D6
    D7 ==> D7
    D8 ==> RS
    D9 ==> EN

For TWI/MJKDZ TWI expander connect to the I2C bus (A4/A5). Check the 
device sub-address. Default is 7 (Expander A0..A2 = 0b111)

