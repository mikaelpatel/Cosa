/**
 * @file Cosa/Watchdog.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Math.hh"
#include "Cosa/Power.hh"
#include "Cosa/Bits.h"

static void watchdog_delay(uint32_t ms) 
{ 
  Watchdog::delay(ms); 
}

static void watchdog_sleep(uint16_t s) 
{ 
  Watchdog::delay(s * 1000L); 
}

Watchdog::InterruptHandler Watchdog::s_handler = NULL;
void* Watchdog::s_env = NULL;

volatile uint32_t Watchdog::s_ticks = 0L;
uint8_t Watchdog::s_prescale;
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
  s_initiated = true;
  ::delay = watchdog_delay;
  ::sleep = watchdog_sleep;
}

void
Watchdog::delay(uint16_t ms)
{
  uint32_t ticks = (ms + (ms_per_tick() / 2)) / ms_per_tick();
  uint8_t key = lock();
  uint32_t stop = s_ticks + (ticks == 0 ? 1 : ticks);
  while (s_ticks != stop) {
    unlock(key);
    yield();
    key = lock();
  }
  unlock(key);
}

ISR(WDT_vect)
{
  Watchdog::s_ticks += 1;
  Watchdog::InterruptHandler handler = Watchdog::s_handler;
  if (handler == NULL) return;
  void* env = Watchdog::s_env;
  handler(env);
}
