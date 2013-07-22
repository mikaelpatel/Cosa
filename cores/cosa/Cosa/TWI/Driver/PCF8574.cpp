/**
 * @file Cosa/TWI/Driver/PCF8574.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/PCF8574.hh"

uint8_t 
PCF8574::read()
{
  if (!twi.begin()) return (0);
  uint8_t res;
  twi.read(m_addr, &res, sizeof(res));
  twi.end();
  return (res & m_ddr);
}

bool
PCF8574::write(uint8_t pin, uint8_t value)
{
  uint8_t mask = _BV(pin & PIN_MASK) & ~m_ddr;
  if (value)
    m_port |= mask;
  else
    m_port &= ~mask;
  if (!twi.begin()) return (false);
  int res = twi.write(m_addr, &m_port, sizeof(m_port));
  twi.end();
  return (res == sizeof(m_port));
}

bool
PCF8574::write(uint8_t value)
{
  if (!twi.begin()) return (false);
  m_port = value & ~m_ddr;
  int res = twi.write(m_addr, &m_port, sizeof(m_port));
  twi.end();
  return (res == sizeof(m_port));
}

bool
PCF8574::write(void* buf, size_t size)
{
  if (!twi.begin()) return (false);
  uint8_t* bp = (uint8_t*) buf;
  size_t n = size;
  while (n--) *bp++ &= ~m_ddr;
  int res = twi.write(m_addr, buf, size);
  twi.end();
  return (res == (int) size);
}
