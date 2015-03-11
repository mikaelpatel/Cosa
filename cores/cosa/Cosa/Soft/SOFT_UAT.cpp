/**
 * @file Cosa/Soft/SOFT_UAT.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "Cosa/Soft/UART.hh"

using namespace Soft;

UAT::UAT(Board::DigitalPin tx) :
  Serial(),
  m_tx(tx, 1),
  m_stops(2),
  m_bits(8),
  m_count((F_CPU / 9600) / 4)
{
}

int
UAT::putchar(char c)
{
  uint16_t data = ((0xff00 | c) << 1);
  uint8_t bits = m_bits + m_stops + 1;
  uint16_t count = m_count;
  synchronized {
    do {
      m_tx._write(data & 0x01);
      _delay_loop_2(count);
      data >>= 1;
    } while (--bits);
  }
  return (c & 0xff);
}

bool
UAT::begin(uint32_t baudrate, uint8_t format)
{
  m_stops = 1 + ((format & STOP2) != 0);
  m_bits = 5 + ((format & DATA8) >> 1);
  m_count = ((F_CPU / baudrate) - I_CPU) / 4;
  if (baudrate > 19600) m_count -= 1;
  return (true);
}
