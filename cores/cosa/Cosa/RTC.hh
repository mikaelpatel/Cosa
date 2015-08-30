/**
 * @file Cosa/RTC.hh
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

#ifndef COSA_RTC_HH
#define COSA_RTC_HH

#include "Cosa/Time.hh"

/**
 * Real-time clock; AVR Timer0 for micro/milli-second timing.
 *
 * @section Limitations
 * Cannot be used together with other classes that use Timer#0.
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
   * @note atomic
   */
  static void time(clock_t sec)
    __attribute__((always_inline))
  {
    synchronized s_sec = sec;
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
   * Return the current clock in micro-seconds.
   * @return micro-seconds.
   */
  static uint32_t micros();

  /**
   * Set the current clock in micro-seconds.
   * @param[in] usec.
   * @note atomic
   */
  static void micros(uint32_t usec)
  {
    synchronized {
      s_uticks = usec;
      s_ms = usec / 1000L;
    }
  }

  /**
   * Set the current clock in milli-seconds.
   * @param[in] usec.
   * @note atomic
   */
  static void millis(uint32_t ms)
  {
    synchronized {
      s_uticks = ms * 1000L;
      s_ms = ms;
    }
  }

  /**
   * Return the current clock in milli-seconds.
   * @return milli-seconds.
   */
  static uint32_t millis()
    __attribute__((always_inline))
  {
    uint32_t res;
    synchronized res = s_ms;
    return (res);
  }

  /**
   * Return the current clock in seconds.
   * @return seconds.
   * @note atomic
   */
  static uint32_t seconds()
    __attribute__((always_inline))
  {
    uint32_t res;
    synchronized res = s_sec;
    return (res);
  }

  /**
   * Return the current clock in seconds.
   * @return seconds.
   * @note atomic
   */
  static clock_t time()
    __attribute__((always_inline))
  {
    return (seconds());
  }

  /**
   * Delay using the real-time clock.
   * @param[in] ms sleep period in milli-seconds.
   */
  static void delay(uint32_t ms);

  /**
   * Delay using the real-time clock until the given condition
   * variable becomes true or timeout. Timeout period zero gives
   * blocking behaviour. Return zero if variable became true otherwise
   * a negative error code (ETIME).
   * @param[in] condvar condition variable.
   * @param[in] ms max wait in milli-seconds.
   * @return zero or negative error code.
   */
  static int await(volatile bool &condvar, uint32_t ms);

  /**
   * Call given interrupt handler when the given time period in
   * micro-seconds has expired.
   * Returns true(1) if scheduled otherwise false(0).
   * @param[in] us micro-seconds before call.
   * @param[in] fn interrupt handler.
   * @param[in] env environment pointer.
   * @return bool.
   */
  static bool expire_in(uint16_t us, InterruptHandler fn, void* env = NULL);

  /**
   * Returns true(1) if expired otherwise false(0).
   * @return bool.
   */
  static bool is_expired()
  {
    return (s_period == 0);
  }

  /**
   * Periodically call given interrupt handler with the given time
   * period in micro-seconds has expired. The maximum timer period is
   * 999 us. Returns true(1) if scheduled otherwise false(0).
   * @param[in] us micro-seconds period.
   * @param[in] fn interrupt handler.
   * @param[in] env environment pointer.
   * @return bool.
   */
  static bool periodic_start(uint16_t us, InterruptHandler fn, void* env = NULL);

  /**
   * Stop periodical call. Returns true(1) if successful otherwise false(0).
   * @return bool.
   */
  static bool periodic_stop();

  /**
   * Set tick update callback function. Allow extension of the
   * interrupt handler for Timers.
   * @param[in] fn interrupt handler.
   * @param[in] env environment pointer.
   * @note atomic
   */
  static void set_on_tick(InterruptHandler fn, void* env = NULL)
  {
    synchronized {
      s_on_tick_fn = fn;
      s_on_tick_env = env;
    }
  }

  /**
   * Enable pin output toggle on tick. The pin is board/hardware
   * defined.
   */
  static void enable_pin_toggle();

  /**
   * Disable pin toggle on tick. Pin is defined to input.
   */
  static void disable_pin_toggle();

private:
  static bool s_initiated;	     	//!< Initiated flag.
  static volatile uint32_t s_uticks; 	//!< Tick counter.
  static volatile uint32_t s_ms;	//!< Milli-seconds counter.
  static volatile uint16_t s_msec;	//!< Milli-seconds fraction.
  static volatile clock_t s_sec;	//!< Seconds counter.

  static InterruptHandler s_on_tick_fn;	//!< Tick callback function.
  static void* s_on_tick_env;		//!< Tick callback enviroment.

  static bool s_periodic;		//!< Periodic flag.
  static volatile uint8_t s_period;	//!< Timer tick to expire.

  static InterruptHandler s_on_expire_fn; //!< Expire callback function.
  static void* s_on_expire_env;		//!< Expire callback enviroment.

  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  RTC() {}

  /** Interrupt Service Routine. */
  friend void TIMER0_COMPA_vect(void);
  friend void TIMER0_COMPB_vect(void);

  /** Timer access. */
  friend class Timer;
};

#endif

