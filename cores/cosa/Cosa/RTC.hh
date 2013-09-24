/**
 * @file Cosa/RTC.hh
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

#ifndef __COSA_RTC_HH__
#define __COSA_RTC_HH__

#include "Cosa/Types.h"

/**
 * Real-time clock; Arduino/ATmega328P Timer0 for micro/milli-
 * second timing.
 */
class RTC {
  friend void TIMER0_COMPA_vect(void);
  friend void TIMER0_COMPB_vect(void);
public:
  /**
   * Milli-second interrupt handler callback function prototype.
   * @param[in] env interrupt handler environment.
   */
  typedef void (*InterruptHandler)(void* env);

private:
  static uint8_t s_initiated;
  static volatile uint32_t s_uticks;
  static volatile uint32_t s_ms;
  static volatile uint16_t s_mticks;
  static volatile uint32_t s_sec;
  static InterruptHandler s_handler;
  static void* s_env;
  
  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  RTC() {}

public:
  /**
   * Start the Real-Time Clock.
   * @param[in] handler of milli-second interrupts.
   * @param[in] env handler environment.
   * @return bool true(1) if successful otherwise false(0).
   */
  static bool begin(InterruptHandler handler = NULL, void* env = NULL);

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
   * Set milli-second timeout interrupt handler.
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
   * Set clock (seconds) to real-time (for instance seconds from a
   * given date).
   * @param[in] sec.
   */
  static void set(uint32_t sec) 
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
  static uint32_t diff(uint32_t x, uint32_t y)
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
  static uint32_t millis();

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
   * Delay using the real-time clock.
   * @param[in] ms sleep period in milli-seconds.
   */
  static void delay(uint16_t ms, uint8_t mode = SLEEP_MODE_IDLE);
};

#endif

