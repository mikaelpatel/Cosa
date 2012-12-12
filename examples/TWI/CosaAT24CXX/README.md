CosaAT24CXX
====

Cosa demonstration of the AT24CXX 2-Wire (TWI) Serial EEPROM driver. Reads and updates 32-bytes in the first page of a AT24CXX device.

Circuit

The Arduino analog pins 4 (SDA) and 5 (SCL) are used for I2C/TWI connection. Pullup resistors are not needed in this example as the internal pullup resistors are enabled.

The serial EEPROM is assumed to have sub-address 0b000. 


