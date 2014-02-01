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

#include "Cosa/Linkage.hh"

/**
 * Real-time clock; Arduino/ATmega328P Timer0 for micro/milli-
 * second timing.
 */
class RTC {
  friend void TIMER0_OVF_vect(void);
public:
  /**
   * Milli-second interrupt handler callback function prototype.
   * @param[in] env interrupt handler environment.
   */
  typedef void (*InterruptHandler)(void* env);

  /**
   * Real-time clock Timer class for scheduling of micro/milli-
   * second callbacks.
   */
  class Timer : protected Link {
    friend void TIMER0_COMPA_vect(void);
    friend void TIMER0_OVF_vect(void);
  private:
    /** Queue of timers */
    static Head s_queue;

    /** Queue tick counter (MSB) */
    volatile static uint32_t s_queue_ticks;

    /** Running state of timer handler */
    volatile static bool s_running;

    /** Timer expire time in micro-seconds (RTC::micros) */
    uint32_t m_expires;

    /**
     * Setup timer counter to trigger in given number of micro-seconds.
     */
    static void setup(uint32_t us);

    /**
     * Execute all expired timers and setup timer for next timeout period.
     */
    static void schedule();

  public:
    /**
     * Construct timer handler, delayed function.
     */
    Timer() : Link(), m_expires(0L) {}

    /**
     * Set timer expire time (absolute time, RTC::micro based).
     * @param[in] us expire time.
     */
    void expire_at(uint32_t us)
    {
      m_expires = us;
    }

    /**
     * Get timer expire time (absolute time, RTC::micro based).
     */
    uint32_t expire_at() const
    {
      return m_expires;
    }

    /**
     * Set timer expire time (relative time, RTC::micro based).
     * @param[in] us expire time.
     */
    void expire_after(uint32_t us)
    {
      m_expires = RTC::micros() + us;
    }

    /**
     * Get timer expire time (relative time, RTC::micro based).
     */
    uint32_t expire_after() const
    {
      return RTC::micros() - m_expires;
    }

    /**
     * Return true if the timer is queued.
     * @return bool.
     */
    bool is_started()
    {
      return (get_pred() != this);
    }

    /**
     * Start timer. Expire time should be set with expires_at(). The
     * member function on_expired() is called when the timeout period
     * has expired.
     */
    void start();

    /**
     * Stop the timer and remove from the RTC::Timer queue.
     */
    void stop();

    /**
     * @override RTC::Timer
     * Timer member function that is called when the timeout period 
     * has expired. This member function must be overridden.
     */
    virtual void on_expired() = 0;
    
    /**
     * Minimum relative expiration in order to be queued for ISR dispatch.
     * NOTE: All timers with an expiration less than this will be
     * immediately dispatched.  The actual elapsed time will be
     * IMMEDIATE_DISPATCH_TIME, possibly much sooner than requested.
     */
    static const uint32_t QUEUED_DISPATCH_TIME;

    /** Actual elapsed time for an immediate dispatch */
    static const uint32_t IMMEDIATE_DISPATCH_TIME = (160 / I_CPU);

  private:
    static uint8_t enter_setup_cycle;
    static uint8_t exit_setup_cycle;
    static uint8_t enter_start_cycle;
    static uint8_t enter_schedule_cycle;
    static uint8_t enter_ISR_cycle;
    static uint8_t enter_on_interrupt_cycle;
//#define RTC_TIMER_MEASURE
#ifdef RTC_TIMER_MEASURE
    static const bool MEASURE = true;
    friend class RTCMeasure;
#else
    static const bool MEASURE = false;
#endif
  };

private:
  static bool s_initiated;
  static volatile uint32_t s_uticks;
  static volatile uint16_t s_ticks;
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
   * Get number of micro-seconds per timer cycle.
   * @return micro-seconds.
   */
  static uint16_t us_per_timer_cycle();

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
  static uint32_t millis()
  {
    return (micros() / 1000L);
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
   * Delay using the real-time clock.
   * @param[in] ms sleep period in milli-seconds.
   * @param[in] mode during sleep (Default SLEEP_MODE_IDLE).
   */
  static void delay(uint16_t ms, uint8_t mode = SLEEP_MODE_IDLE);
};

#endif

