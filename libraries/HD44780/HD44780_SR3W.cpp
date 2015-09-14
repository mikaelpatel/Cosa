/**
 * @file HD44780_SR3W.cpp
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

HD44780::SR3W::SR3W(Board::DigitalPin sda,
		    Board::DigitalPin scl,
		    Board::DigitalPin en) :
  m_port(),
  m_sda(sda),
  m_scl(scl),
  m_en(en)
{
}

bool
HD44780::SR3W::setup()
{
  return (false);
}

void
HD44780::SR3W::write4b(uint8_t data)
{
  m_port.data = data;
  uint8_t value = m_port;
  synchronized {
    m_sda._write(value & 0x20);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x10);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x08);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x04);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x02);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x01);
    m_scl._toggle();
    m_scl._toggle();
    m_en._toggle();
    m_en._toggle();
  }
}

void
HD44780::SR3W::write8b(uint8_t data)
{
  write4b(data >> 4);
  write4b(data);
#if (I_CPU >= 16)
  DELAY(SHORT_EXEC_TIME);
#endif
}

void
HD44780::SR3W::set_mode(uint8_t flag)
{
  m_port.rs = flag;
}

void
HD44780::SR3W::set_backlight(uint8_t flag)
{
  m_port.bt = flag;
}
