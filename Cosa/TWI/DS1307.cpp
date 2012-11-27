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
#include "Cosa/BCD.h"

void 
DS1307::timekeeper_t::to_binary()
{
  uint8_t* buf = (uint8_t*) &seconds;
  for (uint8_t i = 0; i < sizeof(timekeeper_t); i++)
    buf[i] = bcd_to_bin(buf[i]);
}

void 
DS1307::timekeeper_t::to_bcd()
{
  uint8_t* buf = (uint8_t*) &seconds;
  for (uint8_t i = 0; i < sizeof(timekeeper_t); i++)
    buf[i] = bin_to_bcd(buf[i]);
}

void 
DS1307::timekeeper_t::print(IOStream& stream, const char* format)
{
  if (format == 0) format = PSTR("%d-%d-%d %d:%d:%d");
  stream.printf_P(format, year, month, date, hours, minutes, seconds);
}

int
DS1307::read_ram(void* buf, uint8_t size, uint8_t pos)
{
  if (!begin()) return (-1);
  write(ADDR, pos);
  int count = read(ADDR, buf, size);
  end();
  return (count);
}

int
DS1307::write_ram(void* buf, uint8_t size, uint8_t pos)
{
  if (!begin()) return (-1);
  int count = write(ADDR, pos, buf, size);
  end();
  return (count);
}


