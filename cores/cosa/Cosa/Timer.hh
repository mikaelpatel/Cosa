/**
 * @file Cosa/Timer.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#ifndef COSA_TIMER_HH
#define COSA_TIMER_HH

#include "Cosa/Linkage.hh"
#include "Cosa/RTC.hh"

/**
 * Real-time clock Timer class for scheduling of micro/milli-second 
 * callbacks.
 */
class Timer : protected Link {
public:
  /**
   * Construct timer handler, delayed function.
   */
  Timer() :
    Link(), 
    m_expires(0L) 
  {
  }

  /**
   * Start the Timer handler. This member function must be called in
   * the setup to allow Timers. 
   */
  static void begin() 
  { 
    RTC::set(on_interrupt); 
  }

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
   * @override Timer
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

  /**
   * Extend the RTC interrupt handler.
   */
  static void on_interrupt(void* env);

  static uint8_t enter_setup_cycle;
  static uint8_t exit_setup_cycle;
  static uint8_t enter_start_cycle;
  static uint8_t enter_schedule_cycle;
  static uint8_t enter_ISR_cycle;
  static uint8_t enter_on_interrupt_cycle;

#if defined(RTC_TIMER_MEASURE)
  static const bool MEASURE = true;
  friend class RTCMeasure;
#else
  static const bool MEASURE = false;
#endif

  /** Interrupt Service Routines */
  friend void TIMER0_COMPA_vect(void);
  friend void TIMER0_OVF_vect(void);
};
#endif



