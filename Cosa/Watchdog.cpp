/**
 * @file Cosa/Watchdog.cpp
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
 * Watchdog abstraction. Low power timer.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Watchdog.h"

Watchdog::Callback Watchdog::_callback = 0;

volatile uint16_t Watchdog::_ticks = 0;

void* Watchdog::_env = 0;

uint8_t Watchdog::_mode = 0;

uint8_t Watchdog::_prescale = 0;

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
Watchdog::begin(uint16_t ms, Callback fn, uint8_t mode)
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

  // Register the callback function
  _callback = fn;
  _prescale = prescale;
  _mode = mode;
}

void
Watchdog::await()
{
  volatile uint16_t ticks = _ticks;

  // Go to sleep and wait for the next tick. Check for other interrupt
  do {
    cli();
    set_sleep_mode(_mode);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  } while (ticks == _ticks);
}

void
Watchdog::delay(uint16_t ms)
{
  uint16_t ticks = ms / ms_per_tick();
  while (ticks--) await();
}

ISR(WDT_vect)
{
  Watchdog::on_timeout();
}
