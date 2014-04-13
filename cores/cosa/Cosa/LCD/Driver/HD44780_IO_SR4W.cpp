/**
 * @file Cosa/LCD/Driver/HD44780_IO_SR4W.cpp
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/LCD/Driver/HD44780.hh"

bool
HD44780::SR4W::setup()
{
  return (true);
}

void 
HD44780::SR4W::write4b(uint8_t data)
{
  write8b(data);
}

void 
HD44780::SR4W::write8b(uint8_t data)
{
  m_sda.write(data, m_scl);
  synchronized {
    m_sda._write(m_rs);
    m_en._toggle();
    m_en._toggle();
  }
  DELAY(SHORT_EXEC_TIME);
}

void 
HD44780::SR4W::write8n(void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  while (size--) write8b(*bp++);
}

void 
HD44780::SR4W::set_mode(uint8_t flag)
{
  m_rs = flag;
}

void 
HD44780::SR4W::set_backlight(uint8_t flag)
{
  m_bt.write(flag);
}
