/**
 * @file Cosa/RTT.hh
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

#ifndef COSA_RTT_HH
#define COSA_RTT_HH

#include "Cosa/Types.h"
#include "Cosa/Job.hh"
#include "Cosa/Clock.hh"

/**
 * Real-Time Timer (RTT) with micro/milli/seconds timing based on
 * hardware timer. Uses Timer2 when possible to allow low power mode
 * with timer. Alternatively Timer0 is used.
 *
 * @section Limitations
 * Cannot be used together with other classes that use AVR/Timer2/
 * Timer0.
 */
class RTT {
public:
  /**
   * Start the real-time timer.
   * @return bool true(1) if successful otherwise false(0).
   */
  static bool begin();

  /**
   * Stop the real-time timer.
   * @return bool true(1) if successful otherwise false(0).
   */
  static bool end();

  /**
   * Get number of micro-seconds per real-time timer tick.
   * @return micro-seconds.
   */
  static uint16_t us_per_tick();

  /**
   * Get number of micro-seconds per timer cycle.
   * @return micro-seconds.
   */
  static uint16_t us_per_timer_cycle();

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
      s_micros = usec;
      s_millis = usec / 1000L;
    }
  }

  /**
   * Return the current clock in milli-seconds.
   * @return milli-seconds.
   */
  static uint32_t millis();

  /**
   * Set the current clock in milli-seconds.
   * @param[in] usec.
   * @note atomic
   */
  static void millis(uint32_t ms)
  {
    synchronized {
      s_micros = ms * 1000L;
      s_millis = ms;
    }
  }

  /**
   * Returns number of milli-seconds from given start time in
   * milli-seconds.
   * @param[in] start
   * @return number of milli-seconds.
   */
  static uint32_t since(uint32_t start)
    __attribute__((always_inline))
  {
    uint32_t now = millis();
    return (now - start);
  }

  /**
   * Delay using the real-time timer. Installed as the global delay()
   * function when the real-time timer is started with begin().
   * @param[in] ms sleep period in milli-seconds.
   */
  static void delay(uint32_t ms);

  /**
   * Wait for the next real-time timer milli-seconds update.
   */
  static void await()
    __attribute__((always_inline))
  {
    delay(0);
  }

  /**
   * RTT Scheduler for jobs with a delay of 50 us or longer.
   */
  class Scheduler : public Job::Scheduler {
  public:
    /**
     * Construct and register a RTT Job Scheduler. Should be a
     * singleton.
     */
    Scheduler() : Job::Scheduler()
    {
      RTT::s_scheduler = this;
    }

    /**
     * @override{Job::Scheduler}
     * Start given job. Returns true(1) if successful otherwise
     * false(0).
     * @return bool.
     */
    virtual bool start(Job* job);

    /**
     * @override{Job::Scheduler}
     * Dispatch expired jobs. Called from RTT ISR.
     */
    virtual void dispatch();

    /**
     * @override{Job::Scheduler}
     * Return current time in micro-seconds.
     */
    virtual uint32_t time();
  };

  /**
   * Set the real-time timer job scheduler.
   * @param[in] scheduler.
   */
  static void job(RTT::Scheduler* scheduler)
  {
    s_scheduler = scheduler;
  }

  /**
   * Get the real-time timer job scheduler.
   * @return scheduler.
   */
  static RTT::Scheduler* scheduler()
  {
    return (s_scheduler);
  }

  /**
   * RTT Clock for seconds level time base.
   */
  class Clock : public ::Clock {
  public:
    /**
     * Construct and register a RTT Clock. Should be a singleton.
     */
    Clock() : ::Clock()
    {
      RTT::s_clock = this;
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
  static bool s_initiated;	     	//!< Initiated flag.
  static uint32_t s_micros;		//!< Micro-seconds counter.
  static uint32_t s_millis;		//!< Milli-seconds counter.
  static Scheduler* s_scheduler;	//!< Job scheduler.
  static Job* s_job;			//!< Timer job.
  static Clock* s_clock;		//!< Clock.

  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  RTT() {}

  /** Interrupt Service Routine. */
#if defined(TIMER2_COMPA_vect)
  friend void TIMER2_COMPA_vect(void);
  friend void TIMER2_COMPB_vect(void);
#elif defined(TCNT0)
  friend void TIMER0_COMPA_vect(void);
  friend void TIMER0_COMPB_vect(void);
#endif

  /** Scheduler access. */
  friend class RTT::Scheduler;
};

#endif
