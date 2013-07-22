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
#include "Cosa/Power.hh"

// Real-Time Clock configuration
#define COUNT 255
#define PRESCALE 64
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK ((COUNT + 1) * US_PER_TIMER_CYCLE)
#define MS_COUNT ((1000 / US_PER_TIMER_CYCLE) - 1)

// Initiated state
uint8_t RTC::s_initiated = 0;

// Timer ticks counter
volatile uint32_t RTC::s_uticks = 0UL;
volatile uint32_t RTC::s_ms = 0L;
volatile uint16_t RTC::s_mticks = 0;
volatile uint32_t RTC::s_sec = 0L;

// Interrupt handler extension
RTC::InterruptHandler RTC::s_handler = 0;
void* RTC::s_env = 0;

bool
RTC::begin(InterruptHandler handler, void* env)
{
  if (s_initiated) return (false);
  synchronized {
    // Set the compare/top value
    OCR0A = COUNT;
    OCR0B = MS_COUNT;
    // Use clear time on top mode
    TCCR0A = _BV(WGM01);
    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));
    // And enable interrupt on compare match (both A and B)
    TIMSK0 = (_BV(OCIE0A) | _BV(OCIE0B));
    // Reset the counter and clear interrupts
    TCNT0 = 0;
    TIFR0 = 0;
  }
  s_handler = handler;
  s_env = env;
  s_initiated = true;
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

uint16_t 
RTC::us_per_tick()
{
  return (US_PER_TICK);
}
  
uint32_t 
RTC::millis()
{
  uint32_t res;
  uint8_t cnt;
  // Read milli-seconds counter. Adjust if pending interrupt
  synchronized {
    res = s_ms;
    cnt = TCNT0;
    if ((TIFR0 & _BV(OCF0B)) && (cnt < MS_COUNT)) res += 1;
  }
  return (res);
}

uint32_t 
RTC::micros()
{
  uint32_t res;
  uint8_t cnt;
  // Read tick count and hardware counter. Adjust if pending interrupt
  synchronized {
    res = s_uticks;
    cnt = TCNT0;
    if ((TIFR0 & _BV(OCF0A)) && (cnt < COUNT)) res += US_PER_TICK;
  }
  // Convert ticks to micro-seconds
  res += ((uint32_t) cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

void 
RTC::delay(uint16_t ms, uint8_t mode)
{
  uint32_t start = RTC::millis();
  while (RTC::since(start) < ms) Power::sleep(mode);
}

ISR(TIMER0_COMPA_vect)
{
  // Set the top register (again) and increment tick counter
  OCR0A = COUNT;
  RTC::s_uticks += US_PER_TICK;
}

ISR(TIMER0_COMPB_vect)
{
  // Set the top register (again) and increment ms counter
  OCR0B = MS_COUNT;
  RTC::s_ms += 1;
  // Increment ms ticks count and check if seconds should be incremented
  RTC::s_mticks += 1;
  if (RTC::s_mticks == 1000) {
    RTC::s_sec += 1;
    RTC::s_mticks = 0;
  }
  // Check for callback
  RTC::InterruptHandler handler = RTC::s_handler;
  if (handler == 0) return;
  void* env = RTC::s_env;
  handler(env);
}

