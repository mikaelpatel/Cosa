/**
 * @file MCP23008.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "MCP23008.hh"

bool
MCP23008::set_data_direction(uint8_t ddr)
{
  m_iodir = ddr;
  twi.begin(this);
  int res = twi.write((uint8_t) IODIR, &m_iodir, sizeof(m_iodir));
  twi.end();
  return (res == sizeof(m_iodir));
}

bool
MCP23008::set_pullup(uint8_t pur)
{
  m_gppu = pur;
  twi.begin(this);
  int res = twi.write((uint8_t) GPPU, &m_gppu, sizeof(m_gppu));
  twi.end();
  return (res == sizeof(m_gppu));
}

uint8_t
MCP23008::read()
{
  uint8_t res;
  twi.begin(this);
  twi.write((uint8_t) GPIO);
  twi.read(&res, sizeof(res));
  twi.end();
  return (res);
}

bool
MCP23008::write(uint8_t pin, uint8_t value)
{
  uint8_t mask = _BV(pin & PIN_MASK);
  if (value)
    m_olat |= mask;
  else
    m_olat &= ~mask;
  twi.begin(this);
  int res = twi.write((uint8_t) OLAT, &m_olat, sizeof(m_olat));
  twi.end();
  return (res == sizeof(m_olat));
}

bool
MCP23008::write(uint8_t value)
{
  m_olat = value;
  twi.begin(this);
  int res = twi.write((uint8_t) OLAT, &m_olat, sizeof(m_olat));
  twi.end();
  return (res == sizeof(m_olat));
}
