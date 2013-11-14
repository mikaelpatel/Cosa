/**
 * @file Cosa/ProtocolBuffer.cpp
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

#include "Cosa/ProtocolBuffer.hh"
#include "Cosa/Power.hh"

int 
ProtocolBuffer::getchar()
{
  if (m_outs == NULL) return (-1);
  while (!m_outs->available()) Power::sleep(SLEEP_MODE_IDLE);
  return (m_outs->getchar());
}

int 
ProtocolBuffer::read(uint8_t& tag, Type& type)
{
  uint8_t data = getchar();
  tag = (data >> 3);
  type = (Type) (data & 0x7);
  return (type <= FIXED32);
}

int 
ProtocolBuffer::read(int32_t& value)
{
  uint8_t data = getchar();
  int32_t res = (data & 0x7f) >> 1;
  bool more = (data & 0x80);
  bool neg = (data & 1);
  int cnt = 1;
  int pos = 6;
  while (more) {
    data = getchar();
    more = (data & 0x80);
    res |= (((uint32_t) (data & 0x7f)) << pos);
    pos += 7;
    cnt += 1;
    if (cnt > (int) (sizeof(value) + 1)) return (-1);
  }
  if (neg) res = ~res;
  value = res;
  return (cnt);
}

int 
ProtocolBuffer::write(uint8_t tag, uint32_t value)
{
  if (tag & 0xe0) return (-1);
  putchar(tag << 3 | VARINT);
  int res = 1;
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
    if (cnt > (int) (sizeof(value) + 1)) return (-1);
  }
  value = res;
  return (cnt);
}

int 
ProtocolBuffer::read(void* buf, uint8_t count)
{
  uint8_t size = getchar();
  if (size > count) return (-1);
  if (size == 0) return (0);
  int res = size;
  uint8_t* bp = (uint8_t*) buf;
  while (size--) *bp++ = getchar();
  return (res);
}

int 
ProtocolBuffer::write(uint8_t tag, const void* buf, uint8_t count)
{
  if (tag & 0xe0) return (-1);
  if (putchar(tag << 3 | LENGTH_DELIMITED) < 0) return (-1);
  if (putchar(count) < 0) return (-1);
  return (write(buf, count) + 2);
}

int 
ProtocolBuffer::read(float32_t& value)
{
  uint8_t size = sizeof(value);
  uint8_t* bp = (uint8_t*) &value;
  while (size--) *bp++ = getchar();
  return (sizeof(value));
}

int 
ProtocolBuffer::write(uint8_t tag, float32_t value)
{
  if (tag & 0xe0) return (-1);
  if (putchar(tag << 3 | FIXED32) < 0) return (-1);
  return (write(&value, sizeof(value)) + 1);
}
