/**
 * @file Cosa/EEPROM.cpp
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
 * Driver for the ATmega/ATtiny internal EEPROM and abstraction of
 * EEPROM devices.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/EEPROM.hh"
#include <avr/eeprom.h>

bool 
EEPROM::Device::is_ready()
{
  return (eeprom_is_ready());
}

int 
EEPROM::Device::read(void* dest, void* src, size_t size)
{
  uint8_t* d = (uint8_t*) dest;
  uint8_t* s = (uint8_t*) src;
  size_t n = size;
  while (n--) *d++ = eeprom_read_byte(s++);
  return (size);
}

int 
EEPROM::Device::write(void* dest, void* src, size_t size)
{
  uint8_t* d = (uint8_t*) dest;
  uint8_t* s = (uint8_t*) src;
  size_t n = size;
  while (n--) eeprom_write_byte(d++, *s++);
  return (size);
}

EEPROM::Device EEPROM::Device::eeprom;
