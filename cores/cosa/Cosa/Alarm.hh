/**
 * @file Cosa/Alarm.hh
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

#ifndef COSA_ALARM_HH
#define COSA_ALARM_HH

#include "Cosa/Types.h"
#include "Cosa/Job.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/ExternalInterrupt.hh"

/**
 * The Alarm class is an extension of the Periodic job class to allow
 * repeated jobs with seconds (clock_t) as time unit.
 */
class Alarm : public Periodic {
public:
  /**
   * Alarm Scheduler for jobs with a delay of 1 second or longer.
   * The scheduler is implemented as a periodic job scheduled every
   * second by the real-time clock scheduler.
   */
  class Scheduler : public Job::Scheduler, public Periodic {
  public:
    /**
     * Construct Alarm Scheduler with given real-time scheduler
     * for dispatch every second.
     * @param[in] scheduler for periodic dispatch.
     */
    Scheduler(RTC::Scheduler* scheduler) :
      Job::Scheduler(),
      Periodic(scheduler, 1000000UL)
    {}

    /**
     * @override Job::Scheduler
     * Return current time in seconds.
     * @return time.
     */
    virtual uint32_t time()
    {
      return (RTC::seconds());
    }

    /**
     * @override Job
     * Dispatch expired alarms.
     */
    virtual void run()
    {
      dispatch();
    }
  };

  /**
   * Alarm Clock is an external interrupt based job scheduler. The
   * interrupt source should provide an interrupt every second to
   * update the seconds counter.
   */
  class Clock : public Job::Scheduler, public ExternalInterrupt {
  public:
    /**
     * Construct Alarm Clock Job Scheduler with given external
     * interrupt pin and mode.
     * @param[in] pin number.
     * @param[in] mode pin mode (Default ON_RISING_MODE).
     * @param[in] pullup flag (Default false).
     */
    Clock(Board::ExternalInterruptPin pin,
	  InterruptMode mode = ON_RISING_MODE,
	  bool pullup = false) :
      Job::Scheduler(),
      ExternalInterrupt(pin, mode, pullup),
      m_seconds(0L)
    {}

    /**
     * @override Job::Scheduler
     * Return current time in seconds.
     * @return time.
     */
    virtual uint32_t time()
    {
      return (m_seconds);
    }

    /**
     * @override Job::Scheduler
     * Set current time in seconds.
     * @param[in] seconds.
     */
    void time(uint32_t seconds)
    {
      m_seconds = seconds;
    }

    /**
     * @override Interrupt::Handler
     * Increment the seconds counter and dispatch any expired alarms.
     * @param[in] arg argument from interrupt service routine (not used).
     */
    virtual void on_interrupt(uint16_t arg = 0)
    {
      UNUSED(arg);
      m_seconds++;
      dispatch();
    }

  protected:
    uint32_t m_seconds;
  };

  /**
   * Construct alarm with given timeout period in seconds. The maximum
   * alarm period is UINT32_MAX (4294967295) seconds. The given
   * scheduler should use seconds as time unit. Alarm::Scheduler is
   * the default scheduler.
   * @param[in] scheduler for alarms.
   * @param[in] period seconds.
   */
  Alarm(Job::Scheduler* scheduler, uint32_t period) :
    Periodic(scheduler, period)
  {}
};
#endif

