/**
 * @file Cosa/TWI/DS1307.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Driver for the DS1307, 64 X 8, Serial I2C Real-Time Clock.
 * See Maxim Integrated product description.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/DS1307.h"

int8_t
DS1307::read_ram(void* buf, uint8_t size, uint8_t pos)
{
  if (!begin(ADDR)) return (-1);
  write(ADDR, pos);
  int8_t count = read(ADDR, buf, size);
  end();
  return (count);
}

int8_t
DS1307::write_ram(void* buf, uint8_t size, uint8_t pos)
{
  struct {
    uint8_t addr;
    uint8_t data[RAM_MAX];
  } src;
  src.addr = pos;
  memcpy(src.data, buf, size);
  if (!begin(ADDR)) return (-1);
  int8_t count = write(ADDR, &src, size + 1);
  end();
  return (count);
}
