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
MCP23008::begin()
{
  uint8_t config = _BV(SEQOP);
  twi.begin(this);
  int res = twi.write((uint8_t) IOCON, &config, sizeof(config));
  twi.end();
  m_reg = IOCON;
  return (res == sizeof(config));
}

bool
MCP23008::set_data_direction(uint8_t iodir)
{
  m_iodir = iodir;
  twi.begin(this);
  int res = twi.write((uint8_t) IODIR, &m_iodir, sizeof(m_iodir));
  twi.end();
  m_reg = IODIR;
  return (res == sizeof(m_iodir));
}

bool
MCP23008::set_pullup(uint8_t gppu)
{
  m_gppu = gppu;
  twi.begin(this);
  int res = twi.write((uint8_t) GPPU, &m_gppu, sizeof(m_gppu));
  twi.end();
  m_reg = GPPU;
  return (res == sizeof(m_gppu));
}

bool
MCP23008::set_interrupt_pin(uint8_t pin, InterruptMode mode)
{
  uint8_t mask = _BV(pin & PIN_MASK);
  int res;
  switch (mode) {
  case DISABLE:
    m_gpinten &= ~mask;
    m_defval &= ~mask;
    m_intcon &= ~mask;
    break;
  case ON_CHANGE:
    m_gpinten |= mask;
    m_defval &= ~mask;
    m_intcon &= ~mask;
    break;
  case ON_FALLING:
    m_gpinten |= mask;
    m_defval |= mask;
    m_intcon |= mask;
    break;
  case ON_RISING:
    m_gpinten |= mask;
    m_defval &= ~mask;
    m_intcon |= mask;
    break;
  }
  twi.begin(this);
  res = twi.write((uint8_t) GPINTEN, &m_gpinten, sizeof(m_gpinten));
  if (res != sizeof(m_gpinten)) goto error;
  res = twi.write((uint8_t) DEFVAL, &m_defval, sizeof(m_defval));
  if (res != sizeof(m_defval)) goto error;
  res = twi.write((uint8_t) INTCON, &m_intcon, sizeof(m_intcon));
  if (res != sizeof(m_intcon)) goto error;
  twi.end();
  return (true);
 error:
  twi.end();
  return (false);
}

uint8_t
MCP23008::read()
{
  uint8_t res;
  twi.begin(this);
  if (m_reg != GPIO) {
    twi.write((uint8_t) GPIO);
    m_reg = GPIO;
  }
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
  m_reg = OLAT;
  return (res == sizeof(m_olat));
}

bool
MCP23008::write(uint8_t value)
{
  m_olat = value;
  twi.begin(this);
  int res = twi.write((uint8_t) OLAT, &m_olat, sizeof(m_olat));
  twi.end();
  m_reg = OLAT;
  return (res == sizeof(m_olat));
}

bool
MCP23008::write(void* buf, size_t size)
{
  if (size == 0) return (true);
  twi.begin(this);
  int res = twi.write((uint8_t) OLAT, buf, size);
  twi.end();
  uint8_t* bp = (uint8_t*) buf;
  size_t ix = size - 1;
  m_olat = bp[ix];
  m_reg = OLAT;
  return (res == (int) size);
}
