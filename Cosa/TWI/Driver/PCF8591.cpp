/**
 * @file Cosa/TWI/Driver/PCF8591.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#if !defined(__ARDUINO_TINY__)
#include "Cosa/TWI/Driver/PCF8591.hh"

bool
PCF8591::begin(uint8_t cntl)
{
  m_cntl = cntl;
  if (!twi.begin()) return (false);
  twi.write(m_addr, m_cntl);
  sample();
  return (true);
}

bool
PCF8591::convert(uint8_t value)
{
  if (!twi.begin()) return (false);
  int count = twi.write(m_addr, m_cntl, &value, 1);
  twi.end();
  return (count == (sizeof(m_cntl) + sizeof(value)));
}

#endif
