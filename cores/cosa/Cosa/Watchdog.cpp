/**
 * @file Cosa/Watchdog.cpp
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

#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"
#include "Cosa/Bits.h"

Watchdog::InterruptHandler Watchdog::s_handler = NULL;
void* Watchdog::s_env = NULL;

Head Watchdog::s_timeq[Watchdog::TIMEQ_MAX];

volatile uint32_t Watchdog::s_ticks = 0L;
uint8_t Watchdog::s_prescale;
uint8_t Watchdog::s_mode;
bool Watchdog::s_initiated = false;

uint8_t 
Watchdog::as_prescale(uint16_t ms)
{
  // Map milli-seconds to watchdog prescale values
  uint8_t prescale = log2<uint16_t>((ms + 8) >> 5) - 1;
  if (prescale > 9) prescale = 9;
  return (prescale);
}

void 
Watchdog::begin(uint16_t ms, 
		uint8_t mode, 
		InterruptHandler handler, 
		void* env)
{
  // Map milli-seconds to watchdog prescale values
  uint8_t prescale = as_prescale(ms);

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
  s_initiated = true;
}

void 
Watchdog::attach(Link* target, uint16_t ms)
{
  // Map milli-seconds to watchdog time queue index
  uint8_t level = as_prescale(ms);

  // Attach the target to the selected time queue
  s_timeq[level].attach(target);
}

void
Watchdog::delay(uint16_t ms)
{
  uint32_t ticks = (ms + (ms_per_tick() / 2)) / ms_per_tick();
  uint32_t stop = s_ticks + (ticks == 0 ? 1 : ticks);
  uint8_t key = lock();
  while (s_ticks != stop) {
    unlock(key);
    Power::sleep(s_mode);
    key = lock();
  }
  unlock(key);
}

void
Watchdog::push_timeout_events(void* env)
{ 
  uint32_t changed = (s_ticks ^ (s_ticks + 1));
  for (uint8_t i = s_prescale; i < TIMEQ_MAX; i++, changed >>= 1)
    if ((changed & 1) && !s_timeq[i].is_empty())
      Event::push(Event::TIMEOUT_TYPE, &s_timeq[i], i);
}

ISR(WDT_vect)
{
  Watchdog::s_ticks += 1;
  Watchdog::InterruptHandler handler = Watchdog::s_handler;
  if (handler == NULL) return;
  void* env = Watchdog::s_env;
  handler(env);
}
