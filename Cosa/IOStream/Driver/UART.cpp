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
 * @section Description
 * Basic UART device handler with internal buffering.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#if defined(__ARDUINO_TINYX5__)
#include "Cosa/IOStream/Driver/UART.hh"

// Fix: Replace with softserial

bool
UART::begin(uint32_t baudrate, uint8_t format)
{
  m_period = 1000000L / baudrate;
  m_format = format;
  return (true);
}

int 
UART::putchar(char c)
{
  int res = (c & 0xff);
  synchronized {
    m_pin.write(0);
    DELAY(m_period);
    uint8_t bits = m_format & DATA_MASK;
    do {
      m_pin.write(c & 1);
      DELAY(m_period);
      c >>= 1;
    } while (--bits);
    m_pin.write(1);
  }
  DELAY(m_period * 32);
  return (res);
}

UART uart(Board::D0) __attribute__ ((weak));

#else

#include "Cosa/Bits.h"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Power.hh"

static char ibuffer[UART::BUFFER_MAX];
static IOBuffer ibuf(sizeof(ibuffer), ibuffer);

static char obuffer[UART::BUFFER_MAX];
static IOBuffer obuf(sizeof(obuffer), obuffer);

UART uart(0, &ibuf, &obuf);

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

  // Enable transmitter interrupt 
  *UCSRnB() = (_BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0));

  return (true);
}

bool 
UART::end()
{
  // Disable transmitter interrupt
  *UCSRnB() &= ~(_BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0));
  m_obuf->flush();
  m_ibuf->flush();
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

ISR(USART_UDRE_vect)
{
  uart.on_udre_interrupt();
}

ISR(USART_RX_vect)
{
  uart.on_rx_interrupt();
}

#if defined(__ARDUINO_MIGHTY__) 

UART* uart1 = 0;

ISR(USART1_UDRE_vect)
{
  if (uart1 == 0) return;
  uart1->on_udre_interrupt();
}

ISR(USART1_RX_vect)
{
  if (uart1 == 0) return;
  uart1->on_rx_interrupt();
}
#endif

#if defined(__ARDUINO_MEGA__)

UART* uart1 = 0;

ISR(USART1_UDRE_vect)
{
  if (uart1 == 0) return;
  uart1->on_udre_interrupt();
}

ISR(USART1_RX_vect)
{
  if (uart1 == 0) return;
  uart1->on_rx_interrupt();
}

UART* uart2 = 0;

ISR(USART2_UDRE_vect)
{
  if (uart2 == 0) return;
  uart2->on_udre_interrupt();
}

ISR(USART2_RX_vect)
{
  if (uart2 == 0) return;
  uart2->on_rx_interrupt();
}

UART* uart3 = 0;

ISR(USART3_UDRE_vect)
{
  if (uart3 == 0) return;
  uart3->on_udre_interrupt();
}

ISR(USART3_RX_vect)
{
  if (uart3 == 0) return;
  uart3->on_rx_interrupt();
}

#endif
#endif

