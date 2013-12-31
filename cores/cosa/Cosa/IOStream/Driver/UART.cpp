/**
 * @file Cosa/IOStream/Driver/UART.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
#include "Cosa/IOStream/Driver/UART.hh"

#if defined(__ARDUINO_TINY__)
Soft::UART  __attribute__ ((weak)) uart(Board::D1);
#else

#include "Cosa/Bits.h"
#include "Cosa/Power.hh"

#if defined(__ARDUINO_STANDARD_USB__)
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
  if (m_obuf->putchar(c) == IOStream::EOF) {
    if (m_mode == NON_BLOCKING) return (IOStream::EOF);
    do {
      Power::sleep(m_mode);
    } while (m_obuf->putchar(c) == IOStream::EOF);
  }
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

#define UART_ISR_UDRE(vec,nr)			\
ISR(vec ## _UDRE_vect)				\
{						\
  if (UART::uart[nr] == NULL) return;		\
  UART::uart[nr]->on_udre_interrupt();		\
}

#define UART_ISR_RX(vec,nr)			\
  ISR(vec ## _RX_vect)				\
  {						\
    if (UART::uart[nr] == NULL) return;		\
    UART::uart[nr]->on_rx_interrupt();		\
  }

#define UART_ISR_TX(vec,nr)			\
  ISR(vec ## _TX_vect)				\
  {						\
    if (UART::uart[nr] == NULL) return;		\
    UART::uart[nr]->on_tx_interrupt();		\
  }

UART_ISR_UDRE(USART, 0)
UART_ISR_RX(USART, 0)
UART_ISR_TX(USART, 0)

#if defined(__ARDUINO_MIGHTY__) 

UART_ISR_UDRE(USART1, 1)
UART_ISR_RX(USART1, 1)
UART_ISR_TX(USART1, 1)

#elif defined(__ARDUINO_MEGA__)

UART_ISR_UDRE(USART1, 1)
UART_ISR_RX(USART1, 1)
UART_ISR_TX(USART1, 1)

UART_ISR_UDRE(USART2, 2)
UART_ISR_RX(USART2, 2)
UART_ISR_TX(USART2, 2)

UART_ISR_UDRE(USART3, 3)
UART_ISR_RX(USART3, 3)
UART_ISR_TX(USART3, 3)

#endif
#endif

