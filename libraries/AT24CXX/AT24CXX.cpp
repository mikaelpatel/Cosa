/**
 * @file AT24CXX.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "AT24CXX.hh"
#include "Cosa/Power.hh"

bool
AT24CXX::poll(const void* addr, const void* buf, size_t size)
{
  uint8_t i = POLL_MAX;
  int m;
  do {
    twi.acquire(this);
    if (buf == 0) {
      m = twi.write((uint16_t) addr);
      if (m > 0) return (true);
      twi.release();
    }
    else {
      m = twi.write((uint16_t) addr, (void*) buf, size);
      twi.release();
      if (m > 0) return (true);
    }
    delay(16);
  } while (--i);
  return (false);
}

bool
AT24CXX::is_ready()
{
  twi.acquire(this);
  uint16_t addr = 0;
  int m = twi.write(addr);
  twi.release();
  return (m != 0);
}

int
AT24CXX::read(void* dest, const void* src, size_t size)
{
  if (!poll(src)) return (EIO);
  int n = twi.read(dest, size);
  twi.release();
  return (n);
}

int
AT24CXX::write(void* dest, const void* src, size_t size)
{
  size_t s = size;
  uint8_t* q = (uint8_t*) dest;
  uint8_t* p = (uint8_t*) src;
  size_t n = WRITE_MAX - (((uint16_t) dest) & WRITE_MASK);
  if (n > s) n = s;
  if (!poll(q, p, n)) return (EIO);
  s -= n;
  while (s > 0) {
    q += n;
    p += n;
    n = (s < WRITE_MAX ? s : WRITE_MAX);
    if (!poll(q, p, n)) return (EIO);
    s -= n;
  }
  return (size);
}

