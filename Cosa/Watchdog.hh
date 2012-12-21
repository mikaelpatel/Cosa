/**
 * @file Cosa/Watchdog.hh
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

#ifndef __COSA_WATCHDOG_HH__
#define __COSA_WATCHDOG_HH__

#include <avr/wdt.h>
#include <avr/sleep.h>

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/Event.hh"
#include "Cosa/Thing.hh"
#include "Cosa/Things.hh"

class Watchdog {

public:
  /**
   * Interrupt handler function prototype.
   * @param[in] env interrupt handler environment.
   */
  typedef void (*InterruptHandler)(void* env);

  /**
   * Callback function prototype for await condition function.
   * Return true(1) to return from await() otherwise false(0).
   * @param[in] env condition function environment.
   * @return bool
   */
  typedef bool (*AwaitCondition)(void* env);

private:
  static InterruptHandler _handler;
  static void* _env;

  static const uint8_t TIMEQ_MAX = 10;
  static Things _timeq[TIMEQ_MAX];

  static volatile uint16_t _ticks;
  static uint8_t _prescale;
  static uint8_t _mode;
  
public:
  /**
   * Get number of watchdog cycles.
   * @return number of ticks.
   */
  static uint16_t get_ticks() 
  { 
    return (_ticks); 
  }

  /**
   * Reset the ticks counter for time measurement.
   */
  static void reset() 
  { 
    _ticks = 0;
  }

  /**
   * Get number of milli-seconds per tick.
   * @return milli-seconds.
   */
  static uint16_t ms_per_tick() 
  { 
    return (16 << _prescale); 
  }

  /**
   * Set watchdog timeout interrupt handler.
   * @param[in] fn interrupt handler.
   * @param[in] env environment pointer.
   */
  static void set(InterruptHandler fn, void* env = 0) 
  { 
    synchronized {
      _handler = fn; 
      _env = env; 
    }
  }

  /**
   * Set watchdog sleep mode as defined by set_sleep_mode().
   * @param[in] mode sleep mode.
   */
  static void set(uint8_t mode)
  { 
    _mode = mode; 
  }
  
  /**
   * Attach thing to watchdog so that it will receive a timeout
   * event with given period in milli-seconds.
   * @param[in] thing timeout target to add.
   * @param[in] ms milliseconds.
   */
  static void attach(Thing* thing, uint16_t ms);

  /**
   * Start watchdog with given period (milli-seconds) and sleep mode.
   * The timeout period is mapped to 16 milli-seconds and double
   * periods (32, 64, 128, etc to approx 8 seconds).
   * @param[in] ms timeout period in milli-seconds.
   * @param[in] mode sleep mode.
   * @param[in] handler of interrupts.
   * @param[in] env handler environment.
   */
  static void begin(uint16_t ms = 16, 
		    uint8_t mode = SLEEP_MODE_IDLE,
		    InterruptHandler handler = 0,
		    void* env = 0);

  /**
   * Await condition. Put into sleep mode according to  begin
   * setup. Execute function for each wakeup to check if a condition
   * has been valid. If the condition function is null(0) the next
   * tick is awaited. 
   * @param[in] fn function.
   * @param[in] env function environment.
   * @param[in] ms max sleep period in milli-seconds.
   */
  static void await(AwaitCondition fn = 0, void* env = 0, uint16_t ms = 0);

  /**
   * Delay using watchdog timeouts and sleep mode.
   * @param[in] ms sleep period in milli-seconds.
   */
  static void delay(uint16_t ms) { await(0, 0, ms); }
  
  /**
   * Stop watchdog. Turn off timout callback.
   */
  static void end() 
  { 
    wdt_disable(); 
  }

  /**
   * Default interrupt handler for timeout queues.
   * @param[in] env interrupt handler environment.
   */
  static void push_timeout_events(void* env)
  { 
    uint16_t changed = (_ticks ^ (_ticks + 1));
    for (uint8_t i = _prescale; i < TIMEQ_MAX; i++, changed >>= 1)
      if ((changed & 1) && !_timeq[i].is_empty())
	Event::push(Event::TIMEOUT_TYPE, &_timeq[i], i);
  }

  /**
   * Alternative interrupt handler for watchdog events.
   * @param[in] env interrupt handler environment.
   */
  static void push_watchdog_event(void* env)
  { 
    Event::push(Event::WATCHDOG_TYPE, 0, env);
  }

  /**
   * Trampoline function for interrupt service on watchdog timeout.
   */
  static void on_timeout()
  {
    if (_handler != 0) _handler(_env);
    _ticks += 1;
  }
};

#endif
