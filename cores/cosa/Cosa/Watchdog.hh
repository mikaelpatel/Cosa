/**
 * @file Cosa/Watchdog.hh
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

#ifndef COSA_WATCHDOG_HH
#define COSA_WATCHDOG_HH

#include <avr/wdt.h>

#include "Cosa/Types.h"
#include "Cosa/Job.hh"
#include "Cosa/Clock.hh"

/**
 * The AVR Watchdog is used as a low power timer for periodical
 * events and delay. Please note that the accuracy is only within 1-10%
 * if not calibrated.
 */
class Watchdog {
public:
  /**
   * Get initiated state.
   * @return bool.
   */
  static bool is_initiated()
  {
    return (s_initiated);
  }

  /**
   * Get watchdog clock in milli-seconds.
   * @return ms.
   */
  static uint32_t millis()
    __attribute__((always_inline))
  {
    uint32_t res;
    synchronized res = s_millis;
    return (res);
  }

  /**
   * Set watchdog clock in millis-seconds.
   * @param[in] ms.
   */
  static void millis(uint32_t ms)
  {
    synchronized s_millis = ms;
  }

  /**
   * Get number of milli-seconds per tick.
   * @return milli-seconds.
   */
  static uint16_t ms_per_tick()
  {
    return (s_ms_per_tick);
  }

  /**
   * Start watchdog with given period (milli-seconds). The given
   * timeout period is mapped to 16 milli-seconds and double periods
   * (32, 64, 128, etc to approx 8 seconds).
   * @param[in] ms timeout period in milli-seconds (default 16 ms).
   */
  static void begin(uint16_t ms = 16);

  /**
   * Delay using watchdog timeouts and sleep mode.
   * @param[in] ms sleep period in milli-seconds.
   */
  static void delay(uint32_t ms);

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
   * @return milli-seconds.
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
   * Watchdog Scheduler for jobs with milli-seconds level time base.
   */
  class Scheduler : public Job::Scheduler {
  public:
    /**
     * Construct and register a Watchdog Job Scheduler. Should be a
     * singleton.
     */
    Scheduler() : Job::Scheduler()
    {
      Watchdog::s_scheduler = this;
    }

    /**
     * @override{Job::Scheduler}
     * Return current watchdog time in milli-seconds.
     */
    virtual uint32_t time()
    {
      return (Watchdog::millis());
    }
  };

  /**
   * Set the watchdog job scheduler.
   * @param[in] scheduler.
   */
  static void job(Watchdog::Scheduler* scheduler)
  {
    s_scheduler = scheduler;
  }

  /**
   * Get the watchdog job scheduler.
   * @return scheduler.
   */
  static Watchdog::Scheduler* scheduler()
  {
    return (s_scheduler);
  }

  /**
   * Watchdog Clock for seconds level time base.
   */
  class Clock : public ::Clock {
  public:
    /**
     * Construct and register a Watchdog Clock. Should be a singleton.
     */
    Clock() : ::Clock()
    {
      Watchdog::s_clock = this;
    }
  };

  /**
   * Set the wall-clock.
   * @param[in] clock.
   */
  static void wall(Clock* clock)
  {
    s_clock = clock;
  }

  /**
   * Get the wall-clock.
   * @return clock.
   */
  static Clock* clock()
  {
    return (s_clock);
  }

private:
  static bool s_initiated;		//!< Initiated flag.
  static uint32_t s_millis;		//!< Milli-seconds counter.
  static uint16_t s_ms_per_tick;	//!< Number of milli-seconds per tick.
  static Event::Handler* s_handler;	//!< Watchdog timeout event handler.
  static Scheduler* s_scheduler;	//!< Watchdog Job Scheduler.
  static Clock* s_clock;		//!< Watchdog Clock.

  /**
   * Do not allow instances. This is a static singleton.
   */
  Watchdog() {}

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
