/**
 * @file Cosa/Activity.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_ACTIVITY_HH
#define COSA_ACTIVITY_HH

#include "Cosa/Types.h"
#include "Cosa/Time.hh"
#include "Cosa/Alarm.hh"

/**
 * Cosa Activity Handler; the activity run function is called when the
 * activity start time is reached. The function is called with a given
 * run period through the activity duration. The activity is
 * rescheduled with a period.
 * 
 * @sections Examples
 * 1. Starting at 10:00 schedule the activity every hour. Run every
 * minute for 10 minutes.  
 * 2. Starting at 12:00 schedule the activity every 12 hours. Run
 * once. 
 */
class Activity {
public:
  /**
   * Construct activity with default setting; start(now), duration(15 seconds),
   * period(1 minute), and run period(5 seconds). Use member function set_time()
   * to set activity.
   */
  Activity() :
    m_scheduler(this),		// Alarm used as scheduler
    m_start_time(0L),		// Start time (now)
    m_stop_time(15L),		// Stop time (15 seconds)
    m_duration(15),		// Duration (15 seconds)
    m_period(1),		// Period (1 minute)
    m_run_period(5),		// Run period (5 seconds)
    m_cycles(0)			// Cycles
  {
  }
  
  /**
   * Set activity start time, duration in seconds and period in 
   * minutes.
   * @param[in] time to start activity.
   * @param[in] duration of activity in seconds.
   * @param[in] period between activity in minutes.
   */
  void set_time(clock_t time, uint16_t duration, uint16_t period);
  
  /**
   * Set run period when during activity start and duration.
   * @param[in] seconds run period.
   */
  void set_run_period(uint16_t seconds) 
  {
    m_run_period = seconds; 
  }

  /**
   * Return cycle of run in activity.
   * @return cycles.
   */
  uint16_t get_cycles() const
  { 
    return (m_cycles); 
  }

  /**
   * Return time in seconds from scheduler.
   * @return time.
   */
  clock_t time() const
    __attribute__((always_inline))
  { 
    return (Alarm::time()); 
  }

  /**
   * Enable activity handler.
   */
  void enable() 
    __attribute__((always_inline))
  { 
    m_scheduler.enable(); 
  }

  /**
   * Disable activity handler.
   */
  void disable() 
    __attribute__((always_inline))
  { 
    m_scheduler.disable(); 
  }

  /**
   * @override Activity
   * Callback member function during the activity duration. The
   * activity is automatically rescheduled if the activity period is
   * not zero.
   */
  virtual void run() = 0;
  
private:
  /** Activity scheduler */
  class Scheduler : public Alarm {
  public:
    Scheduler(Activity* activity) : 
      Alarm(), 
      m_activity(activity) 
    {}
    virtual void run() 
    { 
      m_activity->schedule(time()); 
    }
  private:
    Activity* m_activity;
  };

  /**
   * Dispatch if activity period has not expired.
   * @param[in] now current time.
   */
  void schedule(clock_t now);
  
  Scheduler m_scheduler;	//!< Activity scheduler.
  clock_t m_start_time;		//!< Start time.
  clock_t m_stop_time;		//!< Stop time.
  uint16_t m_duration;		//!< Duration in seconds.
  uint16_t m_period;		//!< Reschedule period in minutes.
  uint16_t m_run_period;	//!< Run period in seconds.
  uint16_t m_cycles;		//!< Current cycle count.
};
#endif

