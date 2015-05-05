/**
 * @file DS1307.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "DS1307.hh"

int
DS1307::read(void* ram, uint8_t size, uint8_t pos)
{
  twi.begin(this);
  twi.write(pos);
  int count = twi.read(ram, size);
  twi.end();
  return (count);
}

int
DS1307::write(void* ram, uint8_t size, uint8_t pos)
{
  twi.begin(this);
  int count = twi.write(pos, ram, size);
  twi.end();
  return (count);
}
