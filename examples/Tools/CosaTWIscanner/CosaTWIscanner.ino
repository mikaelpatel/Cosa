/**
 * @file CosaTWIscanner.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * Cosa TWI bus scanner; printout connect device and if recognized
 * name of device and short description.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/TWI.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/UART.hh"

/**
 * Device table structure with address, sub-address mask and name/description.
 */
struct tab_t {
  uint8_t addr;			// I2C address
  uint8_t mask;			// Sub-address mask
  uint8_t regaddr;		// Identity register address
  uint8_t id;			// Identity value to match
  const char* name;		// Name and short description
};

/**
 * Device name/description strings in program memory.
 */
const char adxl345[]  __PROGMEM = "ADXL345,Digital Accelermeter";
const char at24cxx[]  __PROGMEM = "AT24CXX,Serial EEPROM";
const char bmp085[]   __PROGMEM = "BMP085,Digital Pressure Sensor";
const char ds1307[]   __PROGMEM = "DS1307,Real-Time Clock";
const char ds2482[]   __PROGMEM = "DS2482, Single-Channel 1-Wire Master";
const char ds3231[]   __PROGMEM = "DS3231,Extremely Accurated RTC/TCXO/Crystal";
const char hmc5883l[] __PROGMEM = "HMC5883L,3-Axis Digital Compass";
const char pcf8574[]  __PROGMEM = "PCF8574,Remote 8-bit I/O Expander";
const char pcf8574a[] __PROGMEM = "PCF8574A,Remote 8-bit I/O Expander";
const char pcf8591[]  __PROGMEM = "PCF8591,8-bit A/D and D/A Converter";
const char l3g4200d[] __PROGMEM = "L3G4200D,3-Axis Digital Gyroscope";
const char mpu60x0[]  __PROGMEM = "MPU-60X0,Motion Processing Unit";
const char si7021[]   __PROGMEM = "Si7021,Humidity and Temperature Sensor";

/**
 * Device table in program memory.
 */
const tab_t dev_tab[] __PROGMEM = {
  { 0x1d, 0xff, 0,    0xe5, adxl345  },
  { 0x1e, 0xff, 0x0a, 0x48, hmc5883l },
  { 0x20, 0xf8, 0,       0, pcf8574  },
  { 0x40, 0xff, 0,       0, si7021   },
  { 0x48, 0xf8, 0,       0, pcf8591  },
  { 0x53, 0xff, 0,    0xe5, adxl345  },
  { 0x50, 0xf8, 0,       0, at24cxx  },
  { 0x68, 0xff, 0x75, 0x68, mpu60x0  },
  { 0x68, 0xfe, 0x0f, 0xd3, l3g4200d },
  { 0x68, 0xff, 0,       0, ds1307   },
  { 0x77, 0xff, 0,       0, bmp085   },
  { 0x70, 0xf8, 0,       0, pcf8574a },
  { 0xd0, 0xff, 0,       0, ds3231   },
  { 0x18, 0xfe, 0,       0, ds2482   },
};

/**
 * Lookup up device given bus address. Return name string in program memory
 * if successful otherwise NULL.
 * @param[in] addr device bus address (with possible sub-address).
 * @param[in] dev twi driver.
 * @return program memory string pointer or NULL.
 */
str_P
lookup(uint8_t addr, TWI::Driver* dev)
{
  // Iterate over all devices in table
  for (uint8_t i = 0; i < membersof(dev_tab); i++) {
    uint8_t dev_addr = pgm_read_byte(&dev_tab[i].addr);
    uint8_t dev_mask = pgm_read_byte(&dev_tab[i].mask);
    // Check if device table address against the parameter address
    if (dev_addr == (addr & dev_mask)) {
      // Check the identity register address
      uint8_t regaddr = pgm_read_byte(&dev_tab[i].regaddr);
      if (regaddr != 0) {
	// Read the register and compare with the expected value
	uint8_t id = 0;
	twi.acquire(dev);
	twi.write(regaddr);
	twi.read(&id, sizeof(id));
	twi.release();
	// Continue with the next table entry if no match
	if (id != pgm_read_byte(&dev_tab[i].id)) continue;
      }
      // Return the name/description string
      return ((str_P) pgm_read_word(&dev_tab[i].name));
    }
  }
  return (NULL);
}

IOStream cout(&uart);

void setup()
{
  // Bind the UART to en output stream for print out
  uart.begin(9600);
  Watchdog::begin();
  cout << PSTR("CosaTWIscanner: started") << endl;
}

void loop()
{
  // Iterate through all bus addresses
  for (uint8_t addr = 3; addr < 128; addr++) {
    // Attempt to read from the device
    TWI::Driver dev(addr);
    twi.acquire(&dev);
    uint8_t data;
    int count = twi.read(&data, sizeof(data));
    twi.release();
    // Continue with the next address if there was no device
    if (count != sizeof(data)) continue;
    // Print information about the device
    cout << PSTR("device = ") << hex << addr
	 << PSTR(":group = ") << (addr >> 3) << '.' << (addr & 0x07);
    str_P name = lookup(addr, &dev);
    if (name != NULL) cout << ':' << name;
    cout << endl;
  }
  cout << endl;
  sleep(5);
}
