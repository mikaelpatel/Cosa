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
Soft::UART  __attribute__ ((weak)) uart(Board::D0);
#else

#include "Cosa/Bits.h"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Power.hh"

static IOBuffer<UART::BUFFER_MAX> ibuf;
static IOBuffer<UART::BUFFER_MAX> obuf;

UART __attribute__ ((weak)) uart(0, &ibuf, &obuf);
UART* UART::uart0 = &uart;

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
  uint8_t mode = SLEEP_MODE_IDLE;
  while (m_obuf->putchar(c) == -1) Power::sleep(mode);
  *UCSRnB() |= _BV(UDRIE0);
  return (c & 0xff);
}

void
UART::on_udre_interrupt()
{
  int c = m_obuf->getchar();
  if (c != -1) 
    *UDRn() = c; 
  else 
    *UCSRnB() &= ~_BV(UDRIE0);
}

void
UART::on_rx_interrupt()
{
  m_ibuf->putchar(*UDRn());
}

#define UART_ISR_UDRE(vec,uart)			\
ISR(vec ## _UDRE_vect)				\
{						\
  if (UART::uart == 0) return;			\
  UART::uart->on_udre_interrupt();		\
}

#define UART_ISR_RX(vec,uart)			\
  ISR(vec ## _RX_vect)				\
  {						\
    if (UART::uart == 0) return;		\
    UART::uart->on_rx_interrupt();		\
  }

UART_ISR_UDRE(USART,uart0)
UART_ISR_RX(USART,uart0)

#if defined(__ARDUINO_MIGHTY__) 

UART* UART::uart1 = 0;
UART_ISR_UDRE(USART1,uart1)
UART_ISR_RX(USART1,uart1)

#elif defined(__ARDUINO_MEGA__)

UART* UART::uart1 = 0;
UART_ISR_UDRE(USART1,uart1)
UART_ISR_RX(USART1,uart1)

UART* UART::uart2 = 0;
UART_ISR_UDRE(USART2,uart2)
UART_ISR_RX(USART2,uart2)

UART* UART::uart3 = 0;
UART_ISR_UDRE(USART3,uart3)
UART_ISR_RX(USART3,uart3)

#endif
#endif

