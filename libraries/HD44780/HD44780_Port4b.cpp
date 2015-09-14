/**
 * @file HD44780_Port4b.cpp
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

#include "Cosa/Board.hh"
#if !defined(BOARD_ATTINYX5)
#include "HD44780.hh"

bool
HD44780::Port4b::setup()
{
  return (false);
}

void
HD44780::Port4b::write4b(uint8_t data)
{
  synchronized {
    m_d0._set(data & 0x01);
    m_d1._set(data & 0x02);
    m_d2._set(data & 0x04);
    m_d3._set(data & 0x08);
    m_en._toggle();
    m_en._toggle();
  }
}

void
HD44780::Port4b::write8b(uint8_t data)
{
  synchronized {
    m_d0._set(data & 0x10);
    m_d1._set(data & 0x20);
    m_d2._set(data & 0x40);
    m_d3._set(data & 0x80);
    m_en._toggle();
    m_en._toggle();
    m_d0._set(data & 0x01);
    m_d1._set(data & 0x02);
    m_d2._set(data & 0x04);
    m_d3._set(data & 0x08);
    m_en._toggle();
    m_en._toggle();
  }
  DELAY(SHORT_EXEC_TIME);
}

void
HD44780::Port4b::set_mode(uint8_t flag)
{
  m_rs.write(flag);
}

void
HD44780::Port4b::set_backlight(uint8_t flag)
{
  m_bt.write(flag);
}
#endif
