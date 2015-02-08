/**
 * @file Cosa/Soft/SOFT_UART.cpp
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

UART::UART(Board::DigitalPin tx, Board::InterruptPin rx, IOStream::Device* ibuf) :
  UAT(tx),
  m_rx(rx, this),
  m_ibuf(ibuf)
{
}

bool
UART::begin(uint32_t baudrate, uint8_t format)
{
  if (!UAT::begin(baudrate, format)) return (false);
  PinChangeInterrupt::begin();
  m_rx.enable();
  return (true);
}

UART::RXPinChangeInterrupt::RXPinChangeInterrupt(Board::InterruptPin pin,
						 UART* uart) :
  PinChangeInterrupt(pin),
  m_uart(uart)
{
}

void
UART::RXPinChangeInterrupt::on_interrupt(uint16_t arg)
{
  UNUSED(arg);
  if (is_set()) return;
  uint16_t count = m_uart->m_count;
  uint8_t bits = m_uart->m_bits;
  uint8_t mask = 1;
  uint8_t data = 0;
  do {
    _delay_loop_2(count);
    if (is_set()) data |= mask;
    mask <<= 1;
  } while (--bits);
  m_uart->m_ibuf->putchar(data);
}
