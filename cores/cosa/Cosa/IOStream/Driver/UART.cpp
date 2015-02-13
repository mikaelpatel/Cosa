/**
 * @file Cosa/IOStream/Driver/UART.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
#include "Cosa/IOStream/Driver/UART.hh"

#if defined(BOARD_ATTINY)
// Default is serial output only
// static IOBuffer<Soft::UART::BUFFER_MAX> ibuf;
// Soft::UART  __attribute__ ((weak)) uart(Board::D2, Board::PCI1, &ibuf);
Soft::UAT  __attribute__ ((weak)) uart(Board::D2);
#else

#include "Cosa/Bits.h"
#include "Cosa/Power.hh"

#if defined(USBCON)
#undef uart
#else
#include "Cosa/IOBuffer.hh"
static IOBuffer<UART::BUFFER_MAX> ibuf;
static IOBuffer<UART::BUFFER_MAX> obuf;
UART __attribute__ ((weak)) uart(0, &ibuf, &obuf);
#endif

UART* UART::uart[Board::UART_MAX] = { 0 };

bool
UART::begin(uint32_t baudrate, uint8_t format)
{
  uint16_t setting = (F_CPU / (baudrate * 8L)) - 1;

  // Check if double rate is not possible
  if (setting > 4095) {
    setting = (F_CPU / (baudrate * 16L)) - 1;
  } else {
    *UCSRnA() = _BV(U2X0);
  }

  // Set baudrate and format
  *UBRRn() = setting;
  *UCSRnC() = format;

  // Enable receiver and transmitter interrupt
  *UCSRnB() = (_BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0));
  return (true);
}

bool
UART::end()
{
  // Disable receiver and transmitter interrupt
  *UCSRnB() &= ~(_BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0));
  return (true);
}

int
UART::putchar(char c)
{
  // Check if the buffer is full
  while (m_obuf->putchar(c) == IOStream::EOF) yield();

  // Enable the transmitter
  *UCSRnB() |= _BV(UDRIE0);
  return (c & 0xff);
}

void
UART::on_udre_interrupt()
{
  int c = m_obuf->getchar();
  if (c != IOStream::EOF)
    *UDRn() = c;
  else {
    *UCSRnB() &= ~_BV(UDRIE0);
    *UCSRnB() |= _BV(TXCIE0);
  }
}

void
UART::on_rx_interrupt()
{
  m_ibuf->putchar(*UDRn());
}

void
UART::on_tx_interrupt()
{
  *UCSRnB() &= ~_BV(TXCIE0);
  on_transmit_completed();
}

#define UART_ISR(vec,nr)			\
ISR(vec ## _UDRE_vect)				\
{						\
  if (UART::uart[nr] == NULL) return;		\
  UART::uart[nr]->on_udre_interrupt();		\
}						\
						\
ISR(vec ## _RX_vect)				\
{						\
  if (UART::uart[nr] == NULL) return;		\
  UART::uart[nr]->on_rx_interrupt();		\
}						\
						\
ISR(vec ## _TX_vect)				\
{						\
  if (UART::uart[nr] == NULL) return;		\
  UART::uart[nr]->on_tx_interrupt();		\
}

#if defined(USART_UDRE_vect)
UART_ISR(USART, 0)
#endif
#if defined(USART1_UDRE_vect)
UART_ISR(USART1, 1)
#endif
#if defined(USART2_UDRE_vect)
UART_ISR(USART2, 2)
#endif
#if defined(USART3_UDRE_vect)
UART_ISR(USART3, 3)
#endif

#endif

