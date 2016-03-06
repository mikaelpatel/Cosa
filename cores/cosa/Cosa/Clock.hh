/**
 * @file Cosa/Clock.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_CLOCK_HH
#define COSA_CLOCK_HH

#include "Cosa/Types.h"
#include "Cosa/Job.hh"

/**
 * Clock for job scheduling with a time unit of seconds. The clock is
 * updated by periodically calling the tick() member function with the
 * clock update in milli-seconds. Started jobs/alarms are dispatched
 * When the clock seconds counter is update.
 */
class Clock : public Job::Scheduler {
public:
  /**
   * Construct clock for job scheduling.
   */
  Clock() :
    Job::Scheduler(),
    m_msec(0),
    m_sec(0UL),
    m_cal(0)
  {}

  /**
   * @override{Job::Scheduler}
   * Return clock time in seconds.
   * @return seconds.
   * @note atomic
   */
  virtual uint32_t time()
  {
    uint32_t res;
    synchronized res = m_sec;
    return (res);
  }

  /**
   * Set clock (seconds) to real-time (for instance seconds from a
   * given date; epoch 1900-01-01 00:00 or 1970-01-01 00:00).
   * @param[in] sec.
   * @note atomic
   */
  void time(uint32_t sec)
  {
    synchronized {
      m_msec = 0;
      m_sec = sec;
    }
  }

  /**
   * Synchronize with clock by waiting for next clock update. Returns
   * clock time in seconds.
   * @return seconds.
   */
  uint32_t await()
  {
    uint32_t start = time();
    uint32_t now;
    while (1) {
      now = time();
      if (now != start) return (now);
      yield();
    }
  }

  /**
   * Increment the clock with the given number of milli-seconds.
   * Dispatch jobs if the clock seconds counter is incremented.
   * @param[in] ms milli-seconds in a tick.
   * @note atomic
   */
  void tick(uint16_t ms)
  {
    synchronized {
      m_msec += ms;
      if (m_msec >= 1000) {
	do {
	  m_msec -= 1000 + m_cal;
	  m_sec += 1;
	} while (m_msec >= 1000);
	dispatch();
      }
    }
  }

  /**
   * Set clock calibration to given number of milli-seconds to adjust
   * by per second.
   * @param[in] ms milli-seconds.
   * @note atomic
   */
  void calibration(int16_t ms)
  {
    synchronized m_cal = ms;
  }

  /**
   * Get current clock calibration in milli-seconds.
   * @return calibration.
   * @note atomic
   */
  int16_t calibration()
  {
    int16_t res;
    synchronized res = m_cal;
    return (res);
  }

  /**
   * Adjust clock calibration with given number of milli-seconds.
   * @param[in] ms milli-seconds.
   * @note atomic
   */
  void adjust(int16_t ms)
  {
    synchronized m_cal += ms;
  }

protected:
  int16_t m_msec;		//!< Milli-seconds fraction.
  uint32_t m_sec;		//!< Seconds counter.
  int16_t m_cal;		//!< Milli-seconds calibration.
};
#endif
