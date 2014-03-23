/**
 * @file Cosa/RTC.hh
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

#ifndef __COSA_RTC_HH__
#define __COSA_RTC_HH__

#include "Cosa/Time.hh"

/**
 * Real-time clock; Arduino/ATmega328P Timer0 for micro/milli-
 * second timing.
 */
class RTC {
public:
  /**
   * RTC interrupt handler function prototype.
   * @param[in] env interrupt handler environment.
   */
  typedef void (*InterruptHandler)(void* env);

  /**
   * Start the Real-Time Clock.
   * @return bool true(1) if successful otherwise false(0).
   */
  static bool begin();

  /**
   * Stop the Real-Time Clock.
   * @return bool true(1) if successful otherwise false(0).
   */
  static bool end();

  /**
   * Get number of micro-seconds per tick.
   * @return micro-seconds.
   */
  static uint16_t us_per_tick();

  /**
   * Get number of micro-seconds per timer cycle.
   * @return micro-seconds.
   */
  static uint16_t us_per_timer_cycle();

  /**
   * Set clock (seconds) to real-time (for instance seconds from a
   * given date; epoch 1900-01-01 00:00 or 1970-01-01 00:00).
   * Please note that the seconds level clock is not not
   * based/calculated from the micro-second level clock. This clock is
   * always from system power up.
   * @param[in] sec.
   */
  static void time(clock_t sec)
  {
    synchronized {
      s_sec = sec;
    }
  }

  /**
   * Returns difference between given time stamps.
   * @param[in] x
   * @param[in] y
   * @return (x - y)
   */
  static int32_t diff(uint32_t x, uint32_t y)
  {
    return (x - y);
  }

  /**
   * Returns number of milli-seconds from given start.
   * @param[in] start
   * @return (millis() - start)
   */
  static uint32_t since(uint32_t start)
  {
    return (millis() - start);
  }

  /**
   * Return the current clock in micro-seconds.
   * @return micro-seconds.
   */
  static uint32_t micros();

  /**
   * Return the current clock in milli-seconds.
   * @return milli-seconds.
   */
  static uint32_t millis()
  {
    return (micros() / 1000);
  }

  /**
   * Return the current clock in seconds.
   * @return seconds.
   */
  static uint32_t seconds()
  {
    uint32_t res;
    synchronized {
      res = s_sec;
    }
    return (res);
  }

  /**
   * Return the current clock in seconds.
   * @return seconds.
   */
  static clock_t time()
  {
    return (seconds());
  }

  /**
   * Delay using the real-time clock.
   * @param[in] ms sleep period in milli-seconds.
   */
  static void delay(uint16_t ms);

  /**
   * Set RTC overflow interrupt handler. Allow extension of the interrupt
   * handler for Timers.
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

private:
  static bool s_initiated;
  static volatile uint32_t s_uticks;
  static volatile uint16_t s_ticks;
  static volatile clock_t s_sec;
  static InterruptHandler s_handler;
  static void* s_env;

  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  RTC() {}

  /** Interrupt Service Routine */
  friend void TIMER0_OVF_vect(void);

  /** Timer access */
  friend class Timer;
};

#endif

