/**
 * @file CosaTWIscanner.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa TWI bus scanner; printout connect device and if recognized 
 * name of device and short description.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"

/**
 * Device table structure with address, sub-address mask and name/description.
 */
struct tab_t {
  uint8_t addr;
  uint8_t mask;
  uint8_t subaddr;
  uint8_t id;
  str_P name;
};

const char adxl345[]  __PROGMEM = "ADXL345, Digital Accelermeter";
const char at24cxx[]  __PROGMEM = "AT24CXX, Serial EEPROM";
const char bmp085[]   __PROGMEM = "BMP085, Digital Pressure Sensor";
const char ds1307[]   __PROGMEM = "DS1307, Real-Time Clock";
const char ds3231[]   __PROGMEM = "DS3231, Extremely Accurated RTC/TCXO/Crystal";
const char hmc5883l[] __PROGMEM = "HMC5883L, 3-Axis Digital Compass";
const char pcf8574[]  __PROGMEM = "PCF8574, Remote 8-bit I/O Expander";
const char pcf8574a[] __PROGMEM = "PCF8574A, Remote 8-bit I/O Expander";
const char pcf8591[]  __PROGMEM = "PCF8591, 8-bit A/D and D/A Converter";
const char l3g4200d[] __PROGMEM = "L3G4200D, 3-Axis Digital Gyroscope";
const char mpu60x0[]  __PROGMEM = "MPU-60X0, Motion Processing Unit";

/**
 * Device table in program memory.
 */
const tab_t devs[] __PROGMEM = {
  { 0x1d, 0xff, 0,    0xe5, adxl345  },
  { 0x1e, 0xff, 0x0a, 0x48, hmc5883l },
  { 0x20, 0xf8, 0,       0, pcf8574  },
  { 0x48, 0xf8, 0,       0, pcf8591  },
  { 0x50, 0xf8, 0,       0, at24cxx  },
  { 0x53, 0xff, 0,    0xe5, adxl345  },
  { 0x68, 0xff, 0x75, 0x68, mpu60x0  },
  { 0x68, 0xfe, 0x0f, 0xd3, l3g4200d },
  { 0x68, 0xff, 0,       0, ds1307   },
  { 0x70, 0xf8, 0,       0, pcf8574a },
  { 0x77, 0xff, 0,       0, bmp085   },
  { 0xd0, 0xff, 0,       0, ds3231   },
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
  for (uint8_t i = 0; i < membersof(devs); i++) {
    if (pgm_read_byte(&devs[i].addr) == (addr & pgm_read_byte(&devs[i].mask))) {
      uint8_t subaddr = pgm_read_byte(&devs[i].subaddr);
      if (subaddr != 0) {
	uint8_t id = 0;
	twi.begin(dev);
	twi.write(subaddr);
	twi.read(&id, sizeof(id));
	twi.end();
	if (id != pgm_read_byte(&devs[i].id)) continue;
      }
      return ((str_P) pgm_read_word(&devs[i].name));
    }
  }
  return (NULL);
}

void setup()
{
  IOStream cout(&uart);
  uart.begin(9600);
  for (uint8_t addr = 3; addr < 128; addr++) {
    TWI::Driver dev(addr);
    twi.begin(&dev);
    uint8_t data;
    int count = twi.read(&data, sizeof(data));
    twi.end();
    if (count == sizeof(data)) {
      cout << PSTR("device = ") << hex << addr 
	   << PSTR(":group = ") << (addr >> 3) << '.' << (addr & 0x07);
      str_P name = lookup(addr, &dev);
      if (name != NULL) cout << ':' << name;
      cout << endl;
    }
  }
}

void loop()
{
}
