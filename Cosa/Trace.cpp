/**
 * @file Cosa/Trace.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
 * Basic trace support class. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Trace.h"
#include "Bits.h"

static const uint8_t BUFFER_MAX = 64;
static const uint8_t BUFFER_MASK = BUFFER_MAX - 1;
static volatile char buffer[BUFFER_MAX];
static volatile uint8_t head;
static volatile uint8_t tail;

static void 
prefix(uint8_t base)
{
  if (base == 2)
    Trace::print_P(PSTR("0b"));
  else if (base == 8)
    Trace::print_P(PSTR("0"));
  else if (base == 16)
    Trace::print_P(PSTR("0x"));
}

bool 
Trace::begin(uint32_t baudrate)
{
  uint16_t setting = (F_CPU / (baudrate * 16L)) - 1;

  // Set up trace buffer
  head = 0;
  tail = 0;
  
  // Check if double rate is needed
  if (setting & 0x8000) {
    UCSR0A = _BV(U2X0);
    setting &= ~0x8000;
  }

  // Set baudrate
  UBRR0H = (uint8_t) (setting >> 8);
  UBRR0L = (uint8_t) setting;

  // Enable transmitter interrupt
  UCSR0B = _BV(TXEN0);
    
  // Set frame format: asynchronous, 8data, 2stop bit
  UCSR0C = _BV(USBS0) | (3 << UCSZ00);
  return (1);
}

bool end()
{
  UCSR0B = 0;
  return (1);
}

void 
Trace::print(int n, uint8_t base) 
{
  if (base != 10) prefix(base);
  char buf[sizeof(int) * CHARBITS + 1];
  print(itoa(n, buf, base));
}

void 
Trace::print(long int n, uint8_t base)
{
  if (base != 10) prefix(base);
  char buf[sizeof(long int) * CHARBITS + 1];
  print(ltoa(n, buf, base));
}

void 
Trace::print(char c)
{
  uint8_t next = (head + 1) & BUFFER_MASK;
  while (next == tail);
  buffer[next] = c;
  head = next;
  bit_set(UCSR0B, UDRIE0);
}

ISR(USART_UDRE_vect)
{
  if (head != tail) {
    uint8_t next = (tail + 1) & BUFFER_MASK;
    tail = next;
    UDR0 = buffer[next];
  }
  else {
    bit_clear(UCSR0B, UDRIE0);
  }
}
