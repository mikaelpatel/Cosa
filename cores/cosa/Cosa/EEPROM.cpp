/**
 * @file Cosa/EEPROM.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/EEPROM.hh"

bool
EEPROM::Device::is_ready()
{
  return (eeprom_is_ready());
}

int
EEPROM::Device::read(void* dest, const void* src, size_t size)
{
  uint8_t* dp = (uint8_t*) dest;
  const uint8_t* sp = (const uint8_t*) src;
  size_t res = size;
  while (size--) *dp++ = eeprom_read_byte(sp++);
  return (res);
}

int
EEPROM::Device::write(void* dest, const void* src, size_t size)
{
  uint8_t* dp = (uint8_t*) dest;
  const uint8_t* sp = (const uint8_t*) src;
  size_t res = size;
  while (size--) eeprom_write_byte(dp++, *sp++);
  return (res);
}

EEPROM::Device EEPROM::Device::eeprom;
