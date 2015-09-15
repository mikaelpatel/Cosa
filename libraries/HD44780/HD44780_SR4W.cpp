/**
 * @file HD44780_SR4W.cpp
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

#include "HD44780.hh"

HD44780::SR4W::SR4W(Board::DigitalPin sda,
		    Board::DigitalPin scl,
		    Board::DigitalPin en,
		    Board::DigitalPin bt) :
  m_sda(sda),
  m_scl(scl),
  m_en(en),
  m_bt(bt, 1),
  m_rs(0)
{
}

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
HD44780::SR4W::set_mode(uint8_t flag)
{
  m_rs = flag;
}

void
HD44780::SR4W::set_backlight(uint8_t flag)
{
  m_bt.write(flag);
}
