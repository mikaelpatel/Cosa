/**
 * @file Cosa/TWI/Driver/AT24CXX.cpp
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
#if !defined(__ARDUINO_TINYX5__)
#include "Cosa/TWI/Driver/AT24CXX.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"

bool 
AT24CXX::poll(void* addr, void* buf, size_t size)
{
  uint8_t i = POLL_MAX;
  int m;
  do {
    if (!twi.begin()) return (false);
    if (buf == 0) {
      m = twi.write(m_addr, (uint16_t) addr);
      if (m != 0) return (true);
      twi.end();
    }
    else {
      m = twi.write(m_addr, (uint16_t) addr, buf, size);
      twi.end();
      if (m != 0) return (true);
    }
    Watchdog::delay(16);
  } while (--i); 
  return (false);
}

bool 
AT24CXX::is_ready()
{
  if (!twi.begin()) return (false);
  uint16_t addr = 0;
  int m = twi.write(m_addr, addr);
  twi.end();
  return (m != 0);
}

int
AT24CXX::read(void* dest, void* src, size_t size)
{
  if (!poll(src)) return (-1);
  int n = twi.read(m_addr, dest, size);
  twi.end();
  return (n);
}

int 
AT24CXX::write(void* dest, void* src, size_t size)
{
  size_t s = size;
  uint8_t* q = (uint8_t*) dest;
  uint8_t* p = (uint8_t*) src;
  size_t n = WRITE_MAX - (((uint16_t) dest) & WRITE_MASK);
  if (n > s) n = s;
  if (!poll(q, p, n)) return (-1);
  s -= n;
  while (s > 0) {
    q += n;
    p += n;
    n = (s < WRITE_MAX ? s : WRITE_MAX);
    if (!poll(q, p, n)) return (-1);
    s -= n;
  }
  return (size);
}
#endif
