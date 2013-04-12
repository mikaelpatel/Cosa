/**
 * @file Cosa/RTC.cpp
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

#include "Cosa/RTC.hh"
#include "Cosa/Board.hh"

// Real-Time Clock: configuration
#define COUNT 255
#define PRESCALE 64
#define TIMER_CYCLES(ticks) ((ticks) << 8)
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK ((COUNT + 1) * US_PER_TIMER_CYCLE)
#define MS_PER_TICK (US_PER_TICK / 1000)

// Timer ticks counter
static volatile uint32_t g_ticks = 0UL;

bool
RTC::begin()
{
  synchronized {
    // Set the compare/top value
    OCR0A = COUNT;
    // Use clear time on top mode
    TCCR0A = _BV(WGM01);
    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));
    // And enable interrupt on compare match
    TIMSK0 = _BV(OCIE0A);
    // Reset the counter and clear interrupts
    TCNT0 = 0;
    TIFR0 = 0;
  }
  return (true);
}

bool
RTC::end()
{
  // Disable the timer interrupts
  synchronized {
    TIMSK0 = 0;
  }
  return (true);
}

uint32_t 
RTC::millis()
{
  uint32_t res;
  // Read tick counter and adjust to milli-seconds
  synchronized {
    res = g_ticks;
  }
  res = res * MS_PER_TICK;
  return (res);
}

uint32_t 
RTC::micros()
{
  uint32_t res;
  uint8_t cnt;
  // Read tick count and hardware counter. Adjust if pending interrupt
  synchronized {
    res = g_ticks;
    cnt = TCNT0;
    if ((TIFR0 & _BV(OCF0A)) && (cnt < COUNT)) res += 1;
  }
  // Convert ticks to micro-seconds
  res = (TIMER_CYCLES(res) + cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

ISR(TIMER0_COMPA_vect)
{
  // Set the top register (again)
  OCR0A = COUNT;
  // Increment tick counter
  g_ticks = g_ticks + 1;
}

