/**
 * @file HD44780_SR3WSPI.cpp
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

HD44780::SR3WSPI::SR3WSPI(Board::DigitalPin en) :
  SPI::Driver(en, SPI::PULSE_HIGH),
  m_port()
{
}

bool
HD44780::SR3WSPI::setup()
{
  return (false);
}

void
HD44780::SR3WSPI::write4b(uint8_t data)
{
  m_port.data = data;
  spi.acquire(this);
    spi.begin();
      spi.transfer(m_port);
    spi.end();
  spi.release();
}

void
HD44780::SR3WSPI::write8b(uint8_t data)
{
  spi.acquire(this);
    spi.begin();
      m_port.data = (data >> 4);
      spi.transfer(m_port);
      m_port.data = data;
      m_cs._toggle();
      m_cs._toggle();
      spi.transfer(m_port);
    spi.end();
  spi.release();
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
