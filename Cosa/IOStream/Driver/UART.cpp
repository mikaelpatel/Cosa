/**
 * @file Cosa/IOStream/Driver/UART.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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

#include "Cosa/Bits.h"
#include "Cosa/IOStream/Driver/UART.hh"

static char buffer[UART::BUFFER_MAX];
static IOBuffer iobuffer(sizeof(buffer), buffer);
UART uart(&iobuffer) __attribute__ ((weak));

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

  // Set baudrate
  *UBRRn() = setting;

  // Enable transmitter interrupt
  *UCSRnB() = _BV(TXEN0);
    
  // Set frame format: asynchronous, 8data, 2stop bit
  *UCSRnC() = format;
  return (1);
}

bool 
UART::end()
{
  // Disable interrupts
  *UCSRnB() &= ~_BV(TXEN0);
  return (1);
}

int 
UART::putchar(char c)
{
  while (m_buffer->putchar(c) == -1);
  *UCSRnB() |= _BV(UDRIE0);
  return (c & 0xff);
}

int 
UART::flush()
{
  return (m_buffer->flush());
}

ISR(USART_UDRE_vect)
{
  int c = uart.m_buffer->getchar();
  if (c != -1) {
    UDR0 = c;
  }
  else {
    bit_clear(UCSR0B, UDRIE0);
  }
}

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#endif

