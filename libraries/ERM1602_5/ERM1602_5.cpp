/**
 * @file ERM1602_5.cpp
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

#include "ERM1602_5.hh"

ERM1602_5::ERM1602_5(Board::DigitalPin sda,
		     Board::DigitalPin scl,
		     Board::DigitalPin en,
		     Board::DigitalPin bt) :
  m_sda(sda),
  m_scl(scl, 1),
  m_en(en, 1),
  m_bt(bt, 1),
  m_rs(0),
  m_dirty(false)
{
}

bool
ERM1602_5::setup()
{
  return (true);
}

void
ERM1602_5::write4b(uint8_t data)
{
  write8b(data);
}

void
ERM1602_5::write8b(uint8_t data)
{
  m_en.clear();
  flush();
  if (m_rs) {
    m_sda.write(HD44780::SET_DDATA_LENGTH, m_scl);
    DELAY(SHORT_EXEC_TIME);
  }
  m_sda.write(data, m_scl);
  m_en.set();
  DELAY(SHORT_EXEC_TIME);
}

void
ERM1602_5::write8n(const void* buf, size_t size)
{
  if (UNLIKELY(size == 0)) return;
  m_en.clear();
  flush();
  if (m_rs) {
    m_sda.write(HD44780::SET_DDATA_LENGTH | ((size - 1) & HD44780::SET_DDATA_MASK), m_scl);
    DELAY(SHORT_EXEC_TIME);
  }
  const uint8_t* bp = (const uint8_t*) buf;
  while (size--) {
    m_sda.write(*bp++, m_scl);
    if (size)
      DELAY(SHORT_EXEC_TIME);
  }
  m_en.set();
  DELAY(SHORT_EXEC_TIME);
}

void
ERM1602_5::set_mode(uint8_t flag)
{
  if (m_rs == flag) return;
  m_rs = flag;
  m_dirty = !m_dirty;
}

void
ERM1602_5::set_backlight(uint8_t flag)
{
  m_bt.write(flag);
}

void
ERM1602_5::flush()
{
  if (!m_dirty) return;
  uint8_t func = HD44780::FUNCTION_SET | HD44780::DATA_LENGTH_8BITS | HD44780::NR_LINES_2;
  if (m_rs) func |= HD44780::EXTENDED_SET;
  m_sda.write(func, m_scl);
  DELAY(SHORT_EXEC_TIME);
  m_dirty = false;
}

