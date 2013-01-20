/**
 * @file Cosa/IOStream/UART.cpp
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

#include "Cosa/IOStream/UART.hh"
#include "Cosa/Bits.h"

UART uart;

bool
UART::begin(uint32_t baudrate)
{
  uint16_t setting = (F_CPU / (baudrate * 8L)) - 1;

  // Set up trace buffer
  m_head = 0;
  m_tail = 0;
  
  // Check if double rate is not possible
  if (setting > 4095) {
    setting = (F_CPU / (baudrate * 16L)) - 1;
  } else {
    UCSR0A = _BV(U2X0);
  }

  // Set baudrate
  UBRR0 = setting;

  // Enable transmitter interrupt
  UCSR0B = _BV(TXEN0);
    
  // Set frame format: asynchronous, 8data, 2stop bit
  UCSR0C = _BV(USBS0) | (3 << UCSZ00);
  return (1);
}

bool 
UART::end()
{
  UCSR0B = 0;
  return (1);
}

int 
UART::putchar(char c)
{
  uint8_t next = (m_head + 1) & BUFFER_MASK;
  while (next == m_tail);
  m_buffer[next] = c;
  m_head = next;
  bit_set(UCSR0B, UDRIE0);
  return (c);
}

int 
UART::flush()
{
  uint32_t cycles = CYCLES_MAX;
  while (m_head != m_tail && cycles != 0) cycles--;
  return (cycles == 0 ? -1 : 0);
}

ISR(USART_UDRE_vect)
{
  if (uart.m_head != uart.m_tail) {
    uint8_t next = (uart.m_tail + 1) & UART::BUFFER_MASK;
    uart.m_tail = next;
    UDR0 = uart.m_buffer[next];
  }
  else {
    bit_clear(UCSR0B, UDRIE0);
  }
}

