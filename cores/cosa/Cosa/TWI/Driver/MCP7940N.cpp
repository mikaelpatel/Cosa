/**
 * @file Cosa/TWI/Driver/MCP7940N.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/TWI/Driver/MCP7940N.hh"
#include "Cosa/BCD.h"

int
MCP7940N::read(void* regs, uint8_t size, uint8_t pos)
{
  if (!twi.begin(this)) return (-1);
  twi.write(pos);
  int count = twi.read(regs, size);
  twi.end();
  return (count);
}

int
MCP7940N::write(void* regs, uint8_t size, uint8_t pos)
{
  if (!twi.begin(this)) return (-1);
  int count = twi.write(pos, regs, size);
  twi.end();
  return (count);
}

IOStream& operator<<(IOStream& outs, MCP7940N::alarm_t& t)
{
  outs << bcd << t.date << ' '
       << bcd << t.hours << ':'
       << bcd << t.minutes << ':'
       << bcd << t.seconds;
  return (outs);
}

IOStream& operator<<(IOStream& outs, MCP7940N::rtcc_t& t)
{
  outs << t.clock << ' '
       << bin << t.control.as_uint8 << ' '
       << t.calibration << ' '
       << t.alarm0 << ' '
       << t.alarm1 << ' ';
  return (outs);
}
