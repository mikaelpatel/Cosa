/**
 * @file ProtocolBuffer.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#include "ProtocolBuffer.hh"
#include "Cosa/Power.hh"

int
ProtocolBuffer::getchar()
{
  if (m_ins == NULL) return (EINVAL);
  while (!m_ins->available()) yield();
  return (m_ins->getchar());
}

int
ProtocolBuffer::write(uint32_t value)
{
  int res = 0;
  do {
    uint8_t data = value & 0x7f;
    value = value >> 7;
    if (value != 0) data |= 0x80;
    putchar(data);
    res += 1;
  } while (value != 0);
  return (res);
}

int
ProtocolBuffer::read(uint32_t& value)
{
  uint8_t data = getchar();
  uint32_t res = (data & 0x7f);
  bool more = (data & 0x80);
  int cnt = 1;
  int pos = 7;
  while (more) {
    data = getchar();
    more = (data & 0x80);
    res |= (((uint32_t) (data & 0x7f)) << pos);
    pos += 7;
    cnt += 1;
    if (cnt > (int) (sizeof(value) + 1)) return (E2BIG);
  }
  value = res;
  return (cnt);
}

int
ProtocolBuffer::write_P(const void* buf, uint8_t count)
{
  int res = count;
  const uint8_t* bp = (const uint8_t*) buf;
  while (count--) putchar(pgm_read_byte(bp++));
  return (res);
}

int
ProtocolBuffer::read(void* buf, uint8_t count)
{
  uint8_t size = getchar();
  if (size > count) return (E2BIG);
  if (size == 0) return (0);
  int res = size;
  uint8_t* bp = (uint8_t*) buf;
  while (size--) *bp++ = getchar();
  return (res);
}

int
ProtocolBuffer::read(float32_t& value)
{
  uint8_t size = sizeof(value);
  uint8_t* bp = (uint8_t*) &value;
  while (size--) *bp++ = getchar();
  return (sizeof(value));
}

