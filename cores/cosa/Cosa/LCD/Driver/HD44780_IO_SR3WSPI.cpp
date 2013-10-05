/**
 * @file Cosa/LCD/Driver/HD44780_IO_SR3WSPI.cpp
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

#include "Cosa/LCD/Driver/HD44780.hh"

bool
HD44780::SR3WSPI::setup()
{
  return (false);
}

void 
HD44780::SR3WSPI::write4b(uint8_t data)
{
  m_port.data = data;
  spi.begin(this);
  spi.transfer(m_port);
  spi.end();
}

void 
HD44780::SR3WSPI::write8b(uint8_t data)
{
  spi.begin(this);
  m_port.data = (data >> 4);
  spi.transfer(m_port);
  m_cs.toggle();
  m_cs.toggle();
  m_port.data = data;
  spi.transfer(m_port);
  spi.end();
  DELAY(SHORT_EXEC_TIME);
}

void 
HD44780::SR3WSPI::set_mode(uint8_t flag)
{
  m_port.rs = flag;
}

void 
HD44780::SR3WSPI::set_backlight(uint8_t flag)
{
  m_port.bt = flag;
}


