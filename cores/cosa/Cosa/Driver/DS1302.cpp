/**
 * @file Cosa/Driver/DS1302.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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

#include "Cosa/Driver/DS1302.hh"

void 
DS1302::write(uint8_t data)
{
  synchronized {
    for (uint8_t bits = 0; bits < CHARBITS; bits++) {
      m_sda.write(data & 0x01);
      DELAY(2);
      data >>= 1;
      m_clk.toggle();
      DELAY(2);
      m_clk.toggle();
    }
  }
  DELAY(2);
}

uint8_t 
DS1302::read()
{
  uint8_t value = 0;
  synchronized {
    for (uint8_t bits = 0; bits < CHARBITS; bits++) {
      value >>= 1;
      if (m_sda.is_set()) value |= 0x80;
      m_clk.set();
      DELAY(2);
      m_clk.clear();
      DELAY(2);
    }
  }
  return (value);
}

void 
DS1302::write(uint8_t addr, uint8_t data)
{
  addr = 0x80 | ((addr & ADDR_MASK) << 1) | WRITE;
  asserted(m_cs) {
    write(addr);
    write(data);
  }
}

uint8_t 
DS1302::read(uint8_t addr)
{
  uint8_t value = 0;
  addr = 0x80 | ((addr & ADDR_MASK) << 1) | READ;
  asserted(m_cs) {
    write(addr);
    m_sda.set_mode(IOPin::INPUT_MODE);
    value = read();
    m_sda.set_mode(IOPin::OUTPUT_MODE);
  }  
  return (value);
}

void 
DS1302::write_protect(bool flag)
{
  write(0x07, flag ? 0x80 : 0x00);
}
  
void 
DS1302::read_ram(void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  if (size > RAM_MAX) size = RAM_MAX;
  asserted(m_cs) {
    write(0xff);
    m_sda.set_mode(IOPin::INPUT_MODE);
    while (size--) *bp++ = read();
    m_sda.set_mode(IOPin::OUTPUT_MODE);
  }  
}

void 
DS1302::write_ram(void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  if (size > RAM_MAX) size = RAM_MAX;
  asserted(m_cs) {
    write(0xfe);
    while (size--) write(*bp++);
  }
}

void 
DS1302::set_time(time_t& now)
{
  write_protect(false);
  write(0, now.seconds);
  write(1, now.minutes);
  write(2, now.hours);
  write(3, now.date);
  write(4, now.month);
  write(5, now.day);
  write(6, now.year);
  write_protect(true);
}

void 
DS1302::get_time(time_t& now)
{
  now.seconds = read(0);
  now.minutes = read(1);
  now.hours = read(2);
  now.date = read(3);
  now.month = read(4);
  now.day = read(5);
  now.year = read(6);
}
