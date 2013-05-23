/**
 * @file Cosa/Watchdog.hh
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

#ifndef __COSA_WATCHDOG_HH__
#define __COSA_WATCHDOG_HH__

#include <avr/wdt.h>
#include <avr/sleep.h>

#include "Cosa/Types.h"
#include "Cosa/Board.hh"
#include "Cosa/Event.hh"
#include "Cosa/Linkage.hh"

/**
 * The Atmega Watchdog is used as a low power timer for period
 * events and delay. 
 */
class Watchdog {
  friend void WDT_vect(void);
public:
  /**
   * Watchdog interrupt handler function prototype.
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
  /**
   * Do not allow instances. This is a static singleton.
   */
  Watchdog() {}

  // Watchdog interrupt handler and environment.
  static InterruptHandler s_handler;
  static void* s_env;

  // Watchdog timeout queues.
  static const uint8_t TIMEQ_MAX = 10;
  static Head s_timeq[TIMEQ_MAX];

  // Watchdog ticks, prescale and mode.
  static volatile uint32_t s_ticks;
  static uint8_t s_prescale;
  static uint8_t s_mode;

public:
  /**
   * Get number of watchdog cycles.
   * @return number of ticks.
   */
  static uint32_t ticks() 
  { 
    return (s_ticks); 
  }

  /**
   * Get Watchdog clock in milli-seconds.
   * @return ms.
   */
  static uint32_t millis() 
  { 
    return (s_ticks * ms_per_tick()); 
  }

  /**
   * Reset the ticks counter for time measurement.
   */
  static void reset() 
  { 
    s_ticks = 0;
  }

  /**
   * Get number of milli-seconds per tick.
   * @return milli-seconds.
   */
  static uint16_t ms_per_tick() 
  { 
    return (16 << s_prescale); 
  }
  
  /**
   * Set watchdog timeout interrupt handler.
   * @param[in] fn interrupt handler.
   * @param[in] env environment pointer.
   */
  static void set(InterruptHandler fn, void* env = 0) 
  { 
    synchronized {
      s_handler = fn; 
      s_env = env; 
    }
  }

  /**
   * Set watchdog sleep mode as defined by set_sleep_mode().
   * @param[in] mode sleep mode.
   */
  static void set(uint8_t mode)
  { 
    s_mode = mode; 
  }
  
  /**
   * Attach target to watchdog so that it will receive a timeout
   * event with given period in milli-seconds.
   * @param[in] target timeout target to add.
   * @param[in] ms milliseconds.
   */
  static void attach(Link* target, uint16_t ms);

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
   * Await condition. Put into sleep mode according to begin()
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
  static void delay(uint16_t ms) 
  { 
    await(0, 0, ms); 
  }
  
  /**
   * Stop watchdog. Turn off timout callback.
   */
  static void end() 
  { 
    wdt_disable(); 
  }

  /**
   * Default interrupt handler for timeout queues; push timeout events
   * to all attached event handlers.
   * @param[in] env interrupt handler environment.
   */
  static void push_timeout_events(void* env);

  /**
   * Alternative interrupt handler for watchdog events; push watchdog
   * event to a main top loop. All attached event handlers are ignored.
   * @param[in] env interrupt handler environment.
   */
  static void push_watchdog_event(void* env)
  { 
    Event::push(Event::WATCHDOG_TYPE, 0, env);
  }
};

#endif
