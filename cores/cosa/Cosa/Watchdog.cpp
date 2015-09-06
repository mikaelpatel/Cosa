/**
 * @file Cosa/Watchdog.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

// Initated flag
bool Watchdog::s_initiated = false;

// Milli-seconds counter and number of ms per tick
volatile uint32_t Watchdog::s_millis = 0L;
uint16_t Watchdog::s_ms_per_tick = 16;

// Watchdog timeout event handler
Event::Handler* Watchdog::s_handler = NULL;

// Watchdog Job Scheduler (milli-seconds level delayed functions)
Watchdog::Scheduler* Watchdog::s_scheduler = NULL;

uint8_t
Watchdog::as_prescale(uint16_t ms)
{
  // Map milli-seconds to watchdog prescale values
  uint8_t prescale = log2<uint16_t>((ms + 8) >> 5);
  if (UNLIKELY(prescale > 9)) prescale = 9;
  return (prescale);
}

void
Watchdog::begin(uint16_t ms)
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

  // Mark as initiated and set watchdog delay
  s_ms_per_tick = (1 << (prescale + 4));
  ::delay = Watchdog::delay;
  s_initiated = true;
}

void
Watchdog::delay(uint32_t ms)
{
  uint32_t start = Watchdog::millis();
  while (since(start) < ms) yield();
}

ISR(WDT_vect)
{
  // Increment milli-seconds counter
  Watchdog::s_millis += Watchdog::s_ms_per_tick;

  // Push timeout event if an event handler is available
  if (UNLIKELY(Watchdog::s_handler != NULL))
    Event::push(Event::TIMEOUT_TYPE, Watchdog::s_handler);

  // Run all expired jobs
  if (Watchdog::s_scheduler != NULL)
    Watchdog::s_scheduler->dispatch();
}
