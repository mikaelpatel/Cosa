/**
 * @file Cosa/Watchdog.cpp
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
 * Watchdog abstraction. Low power timer.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"

Watchdog::InterruptHandler Watchdog::s_handler = 0;
void* Watchdog::s_env = 0;

Things Watchdog::s_timeq[Watchdog::TIMEQ_MAX];

volatile uint16_t Watchdog::s_ticks = 0;
uint8_t Watchdog::s_prescale = 0;
uint8_t Watchdog::s_mode = 0;

inline uint8_t 
log2(uint16_t value)
{
  uint8_t res = 1;
  while (value != 0) {
    res += 1;
    value >>= 1;
  }
  return (res);
}

void 
Watchdog::begin(uint16_t ms, 
		uint8_t mode, 
		InterruptHandler handler, 
		void* env)
{
  // Map milli-seconds to watchdog prescale values
  uint8_t prescale = log2((ms + 8) >> 5) - 1;
  if (prescale > 9) prescale = 9;

  // Create new watchdog configuration
  uint8_t config = _BV(WDIE) | (prescale & 0x07);
  if (prescale > 0x07) config |= _BV(WDP3);

  // Update the watchdog registers
  synchronized {
    wdt_reset();
    bit_clear(MCUSR, WDRF);
    WDTCSR = _BV(WDCE) | _BV(WDE);
    WDTCSR = config;
  }

  // Register the interrupt handler
  s_handler = handler;
  s_env = env;
  s_prescale = prescale;
  s_mode = mode;
}

void 
Watchdog::attach(Thing* thing, uint16_t ms)
{
  uint8_t level = log2((ms + 8) >> 5) - 1;
  if (level > 9) level = 9;
  s_timeq[level].attach(thing);
}

void
Watchdog::await(AwaitCondition fn, void* env, uint16_t ms)
{
  volatile uint16_t ticks = s_ticks + 1;
  if (ms != 0) ticks += (ms / ms_per_tick());
  do {
    if (fn != 0 && fn(env)) return;
    cli();
    set_sleep_mode(s_mode);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  } while ((ticks != s_ticks) || ((ms == 0) && (fn != 0)));
}

ISR(WDT_vect)
{
  Watchdog::on_timeout();
}
