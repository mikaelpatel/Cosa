/**
 * @file Cosa/RTC.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
#define TICKS_PER_SEC (1000000L / US_PER_TICK)

// Initiated state
bool RTC::s_initiated = false;

// Timer ticks counter
volatile uint32_t RTC::s_uticks = 0UL;
volatile uint16_t RTC::s_ticks = 0;
volatile clock_t RTC::s_sec = 0L;

// Timer interrupt extension
RTC::InterruptHandler RTC::s_handler = NULL;
void* RTC::s_env = NULL;

bool
RTC::begin()
{
  if (s_initiated) return (false);
  synchronized {
    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));

    // And enable interrupt on overflow
    TIMSK0 = _BV(TOIE0);

    // Reset the counter and clear interrupts
    TCNT0 = 0;
    TIFR0 = 0;
  }
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

uint16_t
RTC::us_per_timer_cycle()
{
  return (US_PER_TIMER_CYCLE);
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
    if ((TIFR0 & _BV(TOV0)) && cnt < COUNT) res += US_PER_TICK;
  }

  // Convert ticks to micro-seconds
  res += ((uint32_t) cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

void
RTC::delay(uint16_t ms, uint8_t mode)
{
  uint32_t expire = RTC::millis() + ms;
  while (RTC::millis() < expire) Power::sleep(mode);
}

ISR(TIMER0_OVF_vect)
{
  // Increment ticks and check for second count update
  uint16_t ticks = RTC::s_ticks + 1;
  if (ticks == TICKS_PER_SEC) {
    ticks = 0;
    RTC::s_sec += 1;
  }
  RTC::s_ticks = ticks;

  // Increment most significant part of micro second counter
  RTC::s_uticks += US_PER_TICK;

  // Check for extension of the interrupt handler
  RTC::InterruptHandler fn = RTC::s_handler;
  if (fn == NULL) return;
  void* env = RTC::s_env;
  fn(env);
}
