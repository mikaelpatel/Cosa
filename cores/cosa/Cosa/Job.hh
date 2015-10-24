/**
 * @file Cosa/Job.hh
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

#ifndef COSA_JOB_HH
#define COSA_JOB_HH

#include "Cosa/Types.h"
#include "Cosa/Linkage.hh"

/**
 * Abstract job class for handling of scheduled functions. The time
 * scale depends on the queue handler (scheduler). There are three
 * levels of queues predefined. The time resolutions are; seconds
 * (Clock), milli-seconds (Watchdog) and micro-seconds (RTT).
 */
class Job : public Link {
public:
  /**
   * Abstract class for scheduling of jobs. Must be sub-classed to
   * implement time base. The virtual member start() queues jobs,
   * stop() dequeues jobs, dispatch() which typically is called from
   * an interrupt service will by default push a timeout event to the
   * job. The default event handler will call the job run() virtual
   * member function.
   */
  class Scheduler {
  public:
    /**
     * Construct default job scheduler and initiate job queue.
     */
    Scheduler() : m_queue() {}

    /**
     * @override{Job::Scheduler}
     * Start given job. Returns true(1) if successful otherwise
     * false(0).
     * @param[in] job to start.
     * @return bool.
     */
    virtual bool start(Job* job);

    /**
     * @override{Job::Scheduler}
     * Stop given job. Returns true(1) if successful otherwise
     * false(0).
     * @param[in] job to stop.
     * @return bool.
     */
    virtual bool stop(Job* job);

    /**
     * @override{Job::Scheduler}
     * Dispatch expired jobs. This member function is typically called
     * from an interrupt service routine.
     */
    virtual void dispatch();

    /**
     * @override{Job::Scheduler}
     * Return current scheduler time.
     ' @return time.
     */
    virtual uint32_t time() = 0;

  protected:
    /** Job queue. */
    Head m_queue;
  };

  /**
   * Construct delayed job function. The virtual member function run()
   * is called by the given scheduler when the scheduled time expires.
   * @param[in] scheduler for the job.
   */
  Job(Scheduler* scheduler) :
    Link(),
    m_expires(0L),
    m_scheduler(scheduler)
  {}

  /**
   * Set expire time. Absolute time in scheduler time unit.
   * @param[in] time to expire.
   */
  void expire_at(uint32_t time)
  {
    m_expires = time;
  }

  /**
   * Set expire time relative to latest expire time.
   * @param[in] time to add to latest expire time.
   */
  void expire_after(uint32_t time)
  {
    m_expires += time;
  }

  /**
   * Get expire time.
   * @return time.
   */
  uint32_t expire_at() const
  {
    return (m_expires);
  }

  /**
   * Get time remaining before expired.
   * @return time.
   */
  int32_t expire_after() const
  {
    if (m_scheduler != NULL) return (m_expires - m_scheduler->time());
    return (m_expires);
  }

  /**
   * Get current scheduler time. May be used to set relative expire time.
   * @return time.
   */
  uint32_t time() const
  {
    if (m_scheduler != NULL) return (m_scheduler->time());
    return (0UL);
  }

  /**
   * Return true(1) if the job is queued otherwise false(0).
   * @return bool.
   */
  bool is_started() const
    __attribute__((always_inline))
  {
    return (pred() != this);
  }

  /**
   * Start the job. Returns true(1) if scheduled otherwise false(0).
   * @return bool.
   */
  bool start()
  {
    if (UNLIKELY(m_scheduler == NULL)) return (false);
    return (m_scheduler->start(this));
  }

  /**
   * Stop the job. Returns true(1) if scheduled otherwise false(0).
   * @return bool.
   */
  bool stop()
  {
    if (UNLIKELY(m_scheduler == NULL)) return (true);
    return (m_scheduler->stop(this));
  }

  /**
   * @override{Job}
   * Job member function that is called Scheduler::dispatch() when the
   * job time has expired. This function is normally called from an
   * interrupt service routine. The default implementation will push a
   * timeout event with the job as target. The default event handler
   * will call  the job run() virtual member function. Override this
   * function if the job should be executed during the interrupt
   * service routine.
   */
  virtual void on_expired()
  {
    Event::push(Event::TIMEOUT_TYPE, this);
  }

  /**
   * @override{Event::Handler}
   * Default job event handler; execute the run() virtual member
   * function on timeout event. Event is pushed by on_expired().
   * @param[in] type the type of event (Typically TIMEOUT_TYPE).
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    UNUSED(value);
    if (type == Event::TIMEOUT_TYPE) run();
  }

  /**
   * @override{Job}
   * The job run() virtual member function; sub-class should define.
   * Called by the scheduler (via event handler) when the time expires.
   * May set an new expire time and start the job again, or even start
   * other jobs.
   */
  virtual void run() {}

protected:
  /** Expire time. Scale (us, ms, s) depends on scheduler. */
  uint32_t m_expires;

  /** Job scheduler. */
  Scheduler* m_scheduler;
};
#endif
