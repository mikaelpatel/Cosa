/**
 * @file SainSmart_LCD2004.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "SainSmart_LCD2004.hh"

bool
SainSmart_LCD2004::setup()
{
  data_direction(0);
  return (false);
}

void
SainSmart_LCD2004::write4b(uint8_t data)
{
  uint8_t buf[2];
  m_port.data = data;
  m_port.en = 1;
  buf[0] = m_port;
  m_port.en = 0;
  buf[1] = m_port;
  write(buf, sizeof(buf));
}

void
SainSmart_LCD2004::write8b(uint8_t data)
{
  uint8_t buf[4];
  m_port.data = (data >> 4);
  m_port.en = 1;
  buf[0] = m_port;
  m_port.en = 0;
  buf[1] = m_port;
  m_port.data = data;
  m_port.en = 1;
  buf[2] = m_port.as_uint8;
  m_port.en = 0;
  buf[3] = m_port.as_uint8;
  write(buf, sizeof(buf));
}

void
SainSmart_LCD2004::write8n(const void* buf, size_t size)
{
  const uint8_t* bp = (const uint8_t*) buf;
  while (size != 0) {
    uint8_t tmp[TMP_MAX];
    uint8_t n = (size > sizeof(tmp) / 4 ? sizeof(tmp) / 4 : size);
    size -= n;
    uint8_t m = n * 4;
    for (uint8_t i = 0; i < m;) {
      uint8_t data = *bp++;
      m_port.data = (data >> 4);
      m_port.en = 1;
      tmp[i++] = m_port;
      m_port.en = 0;
      tmp[i++] = m_port;
      m_port.data = data;
      m_port.en = 1;
      tmp[i++] = m_port;
      m_port.en = 0;
      tmp[i++] = m_port;
    }
    write(tmp, m);
  }
}

void
SainSmart_LCD2004::set_mode(uint8_t flag)
{
  m_port.rs = flag;
}

void
SainSmart_LCD2004::set_backlight(uint8_t flag)
{
  m_port.bt = flag;
  write(m_port.as_uint8);
}

