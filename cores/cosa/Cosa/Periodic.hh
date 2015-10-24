/**
 * @file Cosa/Periodic.hh
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

#ifndef COSA_PERIODIC_HH
#define COSA_PERIODIC_HH

#include "Cosa/Types.h"
#include "Cosa/Job.hh"

/**
 * Periodic function handler. Syntactic sugar for periodical jobs.
 * Subclass and implement the virtual method run() as the function to
 * be executed periodically. The scheduler defines the time base;
 * the alarm scheduler uses seconds, the watchdog job scheduler
 * milli-seconds and the real-time timer micro-seconds.
 *
 * @section See Also
 * For details on handling of time units see Job.hh. This execution
 * pattern is also available in the FSM (Finite State Machine) class.
 */
class Periodic : public Job {
public:
  /**
   * Construct a periodic function handled by the given scheduler and
   * with the given period in the schedulers time base. The maximum
   * period is UINT32_MAX; 1.2 hours with RTT::Scheduler, 49 days with
   * Watchdog::Scheduler and 136 years with Alarm::Clock.
   * @param[in] scheduler for the periodic job.
   * @param[in] period of timeout.
   */
  Periodic(Job::Scheduler* scheduler, uint32_t period) :
    Job(scheduler),
    m_period(period)
  {}

  /**
   * Set timeout period.
   * @param[in] time period of timeout.
   */
  void period(uint32_t time)
    __attribute__((always_inline))
  {
    m_period = time;
  }

  /**
   * Get timeout period.
   * @return period of timeout.
   */
  uint32_t period() const
    __attribute__((always_inline))
  {
    return (m_period);
  }

  /**
   * Reschedule after a new period.
   */
  void reschedule()
  {
    if (m_period == 0) return;
    expire_after(m_period);
    start();
  }

protected:
  /**
   * @override{Event::Handler}
   * Periodic event handler; dispatch the run() function on
   * timeout events and reschedule the periodic job.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    UNUSED(value);
    if (UNLIKELY(type != Event::TIMEOUT_TYPE)) return;
    run();
    reschedule();
  }

  /** Time period. Time unit is defined by the scheduler. */
  uint32_t m_period;
};

/**
 * Syntactic sugar for periodic blocks in the loop() function. Used in
 * the form:
 * @code
 * void loop()
 * {
 *   periodic(timer,ms) {
 *     ...
 *   }
 *   ...
 * }
 * @endcode
 * May be used several times in the loop() function. The timer
 * variable is defined and available in the loop().
 * @param[in] timer variable.
 * @param[in] ms period in milli-seconds.
 * @note requires RTT.
 */
#define periodic(timer,ms)						\
  static uint32_t timer = RTT::millis();				\
  for (int __UNIQUE(i) = 1;						\
       (__UNIQUE(i) != 0) && ((RTT::since(timer)) >= ms);		\
       __UNIQUE(i)--, timer += ms)

/**
 * Syntactic sugar for periodic blocks in the loop() function. Used in
 * the form:
 * @code
 * void loop()
 * {
 *   PERIODIC(timer,us) {
 *     ...
 *   }
 *   ...
 * }
 * @endcode
 * May be used several times in the loop() function. The timer
 * variable is defined and available in the loop().
 * @param[in] timer variable.
 * @param[in] us period in micro-seconds.
 * @note requires RTT.
 */
#define PERIODIC(timer,us)						\
  static uint32_t timer = RTT::micros();				\
  for (int __UNIQUE(i) = 1;						\
       (__UNIQUE(i) != 0) && ((RTT::micros() - timer) >= us);		\
       __UNIQUE(i)--, timer += us)

#endif
