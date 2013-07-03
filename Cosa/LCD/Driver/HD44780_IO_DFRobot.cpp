/**
 * @file Cosa/LCD/Driver/HD44780_IO_DFRobot.cpp
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

#include "Cosa/Board.hh"
#if !defined(__ARDUINO_TINY__)
#include "Cosa/LCD/Driver/HD44780.hh"

void 
HD44780::DFRobot::setup()
{
  m_port.as_uint8 = 0;
  set_data_direction(0);
}

void 
HD44780::DFRobot::write4b(uint8_t data)
{
  uint8_t buf[2];
  m_port.data = data;
  m_port.en = 1;
  buf[0] = m_port.as_uint8;
  m_port.en = 0;
  buf[1] = m_port.as_uint8;
  write(buf, sizeof(buf));
}

void 
HD44780::DFRobot::write8b(uint8_t data)
{
  uint8_t buf[4];
  m_port.data = (data >> 4);
  m_port.en = 1;
  buf[0] = m_port.as_uint8;
  m_port.en = 0;
  buf[1] = m_port.as_uint8;
  m_port.data = data;
  m_port.en = 1;
  buf[2] = m_port.as_uint8;
  m_port.en = 0;
  buf[3] = m_port.as_uint8;
  write(buf, sizeof(buf));
}

void 
HD44780::DFRobot::set_mode(uint8_t flag)
{
  m_port.rs = flag;
}

void 
HD44780::DFRobot::set_backlight(uint8_t flag)
{
  m_port.bt = flag;
  write(m_port.as_uint8);
}
#endif


