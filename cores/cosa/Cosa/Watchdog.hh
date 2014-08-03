/**
 * @file Cosa/Watchdog.hh
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

#ifndef COSA_WATCHDOG_HH
#define COSA_WATCHDOG_HH

#include <avr/wdt.h>

#include "Cosa/Types.h"
#include "Cosa/Event.hh"
#include "Cosa/Linkage.hh"

/**
 * The AVR Watchdog is used as a low power timer for periodical
 * events and delay. 
 */
class Watchdog {
public:
  /**
   * Watchdog interrupt handler function prototype.
   * @param[in] env interrupt handler environment.
   */
  typedef void (*InterruptHandler)(void* env);

  /**
   * Get initiated state.
   * @return bool.
   */
  static bool is_initiated()
  {
    return (s_initiated);
  }

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
    __attribute__((always_inline))
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
  static void set(InterruptHandler fn, void* env = NULL) 
  { 
    synchronized {
      s_handler = fn; 
      s_env = env; 
    }
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
   * @param[in] handler of interrupts.
   * @param[in] env handler environment.
   */
  static void begin(uint16_t ms = 16, 
		    InterruptHandler handler = NULL,
		    void* env = NULL);

  /**
   * Delay using watchdog timeouts and sleep mode.
   * @param[in] ms sleep period in milli-seconds.
   */
  static void delay(uint16_t ms);

  /**
   * Wait for the next watchdog timeout.
   */
  static void await()
    __attribute__((always_inline))
  {
    delay(ms_per_tick());
  }

  /**
   * Returns number of milli-seconds from given start.
   * @param[in] start
   * @return (millis() - start)
   */
  static uint32_t since(uint32_t start)
    __attribute__((always_inline))
  {
    uint32_t now = millis();
    return (now - start);
  }

  /**
   * Stop watchdog. Turn off timout callback. May be restarted with begin(). 
   */
  static void end() 
  { 
    wdt_disable(); 
    s_initiated = false;
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

private:
  /**
   * Do not allow instances. This is a static singleton.
   */
  Watchdog() {}

  // Watchdog interrupt handler and environment.
  static InterruptHandler s_handler;
  static void* s_env;

  // Watchdog timeout queues (16, 32, ..., 8192 ms)
  static const uint8_t TIMEQ_MAX = 10;
  static Head s_timeq[TIMEQ_MAX];

  // Watchdog ticks, prescale and mode.
  static volatile uint32_t s_ticks;
  static uint8_t s_prescale;
  static bool s_initiated;

  /**
   * Calculate watchdog prescale given timeout period (in milli-seconds).
   * @param[in] ms timeout period.
   * @return prescale factor.
   */
  static uint8_t as_prescale(uint16_t ms);

  /** Interrupt Service Routine. */
  friend void WDT_vect(void);
};

#endif
