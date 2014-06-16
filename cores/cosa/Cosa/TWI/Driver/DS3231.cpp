/**
 * @file Cosa/TWI/Driver/DS3231.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#include "Cosa/TWI/Driver/DS3231.hh"
#include "Cosa/BCD.h"

int
DS3231::read(void* regs, uint8_t size, uint8_t pos)
{
  if (!twi.begin(this)) return (-1);
  twi.write(pos);
  int count = twi.read(regs, size);
  twi.end();
  return (count);
}

int
DS3231::write(void* regs, uint8_t size, uint8_t pos)
{
  if (!twi.begin(this)) return (-1);
  int count = twi.write(pos, regs, size);
  twi.end();
  return (count);
}

bool 
DS3231::get(void* alarm, uint8_t size, uint8_t offset, uint8_t& mask)
{
  int res = read(alarm, size, offset);
  if (res != size) return (false);
  mask = 0;
  uint8_t* buf = (uint8_t*) alarm;
  for (uint8_t i = 0; i < size; i++) {
    if (buf[i] & 0x80) {
      buf[i] &= 0x78;
      mask |= (1 << i);
    }
  }
  return (true);
}

bool 
DS3231::set(void* alarm, uint8_t size, uint8_t offset, uint8_t mask)
{
  uint8_t* buf = (uint8_t*) alarm;
  for (uint8_t i = 0; i < size; i++) {
    if (mask & (1 << i)) {
      buf[i] |= 0x80;
    }
  }
  int res = write(alarm, size, offset);
  return (res == size);
}

int16_t 
DS3231::get_temperature()
{
  int16_t temp = 0;
  read(&temp, sizeof(temp), offsetof(timekeeper_t, temp));
  return (swap(temp) >> 6);
}

IOStream& operator<<(IOStream& outs, DS3231::alarm1_t& t)
{
  outs << bcd << t.date << ' '
       << bcd << t.hours << ':'
       << bcd << t.minutes << ':'
       << bcd << t.seconds;
  return (outs);
}

IOStream& operator<<(IOStream& outs, DS3231::alarm2_t& t)
{
  outs << bcd << t.date << ' '
       << bcd << t.hours << ':'
       << bcd << t.minutes;
  return (outs);
}

IOStream& operator<<(IOStream& outs, DS3231::timekeeper_t& t)
{
  int16_t temp = swap(t.temp) >> 6;
  outs << t.clock << ' '
       << t.alarm1 << ' '
       << t.alarm2 << ' '
       << bin << t.control.as_uint8 << ' '
       << bin << t.status.as_uint8 << ' '
       << t.aging << ' '
       << (temp >> 2) << '.' << (25 * (temp & 0x3));
  return (outs);
}
