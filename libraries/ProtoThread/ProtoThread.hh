/**
 * @file ProtoThread.hh
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

#ifndef COSA_PROTO_THREAD_HH
#define COSA_PROTO_THREAD_HH

#include "Cosa/Job.hh"
#include "Cosa/Event.hh"

/**
 * Cosa implementation of protothreads; A protothread is a
 * low-overhead mechanism for concurrent programming. Protothreads
 * function as stackless, lightweight threads providing a blocking
 * context using minimal memory per protothread. Cosa/Thread supports
 * event to thread mapping and timers.
 *
 * @section Limitations
 * The thread macro set should only be used within the
 * ProtoThread::on_run() function. The macros cannot be used in
 * functions called from on_run().
 *
 * @section Acknowledgements
 * Inspired by research and prototype by Adam Dunkels, Oliver Schmidt,
 * Thiermo Voigt, Muneeb Ali, and the protothread library by Larry
 * Ruane.
 *
 * @section References
 * [1] Adam Dunkels et al, Protothreads: Simplifying Event-Driven
 * Programming of Memory-Constrained Embedded Systems, SenSys'06,
 * November 1-3, 2006, Boulder, Colorado, USA.<br>
 * [2] Larry Ruane, protothread: An extremely lightweight thread
 * library for GCC, http://code.google.com/p/protothread/<br>
 * [3] http://en.wikipedia.org/wiki/Protothreads<br>
 */
class ProtoThread : public Job {
public:
  /**
   * Thread states.
   */
  enum {
    INITIATED = 0,		//!< Constructor.
    READY,			//!< In run queue.
    WAITING,			//!< In timer queue.
    TIMEOUT,			//!< Timeout received and running.
    RUNNING,			//!< Dispatched and running.
    SLEEPING,			//!< Detached. Need wakeup call.
    TERMINATED = 0xff,		//!< Removed from all queues.
  } __attribute__((packed));

  /**
   * Construct thread, initiate state and continuation. Does not
   * schedule the thread. This is done with begin().
   * @param[in] scheduler wiht milli-seconds time unit.
   */
  ProtoThread(Job::Scheduler* scheduler) :
    Job(scheduler),
    m_state(INITIATED),
    m_ip(0)
  {}

  /**
   * Start the thread. Must be in INITIATED state to be allowed to be
   * scheduled. Returns false if not in correct state.
   * @return bool true if scheduled otherwise false.
   */
  bool begin()
  {
    if (UNLIKELY(m_state != INITIATED)) return (false);
    schedule(this);
    return (true);
  }

  /**
   * End the thread. Mark as terminated and remove from any queue.
   * Use macro THREAD_END() in thread body (run function).
   */
  void end()
  {
    m_state = TERMINATED;
    detach();
  }

  /**
   * Get current thread state.
   * @return state.
   */
  uint8_t state() const
  {
    return (m_state);
  }

  /**
   * Set timer and enqueue thread to receive timeout event.
   * If the timer expires the thread is put in TIMEOUT state.
   * @param[in] ms timeout period.
   */
  void set_timer(uint16_t ms)
    __attribute__((always_inline))
  {
    m_state = WAITING;
    detach();
    expire_after(ms);
    start();
  }

  /**
   * Cancel timer and dequeue thread from timer queue.
   */
  void cancel_timer()
    __attribute__((always_inline))
  {
    detach();
  }

  /**
   * Check if the timer expired; i.e., the thread is in TIMEOUT
   * state.
   */
  bool timer_expired() const
    __attribute__((always_inline))
  {
    return (m_state == TIMEOUT);
  }

  /**
   * @override{ProtoThread}
   * Thread activity. Must be overridden. Use the thread macro set in
   * the following format:
   * {
   *   PROTO_THREAD_BEGIN();
   *   while (1) {
   *     ...
   *     PROTO_THREAD_AWAIT(condition);
   *     ...
   *   }
   *   PROTO_THREAD_END();
   * }
   * Additional macros are PROTO_THREAD_YIELD(), PROTO_THREAD_SLEEP(),
   * PROTO_THREAD_WAKE(), and PROTO_THREAD_DELAY().
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_run(uint8_t type, uint16_t value) = 0;

  /**
   * Run threads in the run queue. If given flag is true events will
   * be processes. Returns number of dispatched threads and events.
   * The run queue is only iterated once per call to dispatch to allow
   * user defined outer loop, i.e., arduino loop() function.
   * @param[in] flag process events if non zero.
   * @return number of dispatched threads and events.
   */
  static uint16_t dispatch(bool flag = true);

  /**
   * Add the given thread to the run queue (last). A terminated thread
   * may be restarted.
   * @param[in] thread to enqueue.
   */
  static void schedule(ProtoThread* thread);

protected:
  static Head runq;
  uint8_t m_state;
  void* m_ip;

  /**
   * @override{Event::Handler}
   * The first level event handler. Filters timeout events and
   * run the thread action function.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
};

/**
 * First statement in the thread body, run(). Last statement should be
 * PROTO_THREAD_END();
 */
#define PROTO_THREAD_BEGIN()				\
  if (m_ip != 0) goto *m_ip

/**
 * Yield execution to other threads ane event handlers. Remains in the
 * run queue.
 */
#define PROTO_THREAD_YIELD() 				\
  do {							\
    __label__ next;					\
    m_ip = &&next;					\
    return;						\
  next: ;						\
  } while (0)

/**
 * Yield execution and detach from the run queue. Must be activated
 * with PROTO_THREAD_WAkE().
 */
#define PROTO_THREAD_SLEEP()				\
  do {							\
    m_state = SLEEPING;					\
    detach();						\
    PROTO_THREAD_YIELD();				\
  } while (0)

/**
 * Schedule the given thread if SLEEPING.
 * @param[in] thread to wake.
 */
#define PROTO_THREAD_WAKE(thread)			\
  do {							\
    if (thread->m_state == SLEEPING)			\
      Thread::schedule(thread);				\
  } while (0)

/**
 * Check if the given condition is true(1). If not the thread will
 * yield. The condition is rechecked when the thread is activated
 * again.
 * @param[in] condition to evaluate.
 */
#define PROTO_THREAD_AWAIT(condition)			\
  do {							\
    __label__ next;					\
  next:							\
    if (!(condition)) {					\
      m_ip = &&next;					\
      return;						\
    }							\
  } while (0)

/**
 * Delay the thread for the given ms time period. This is a short form
 * for set_timer() and THREAD_AWAIT(timer_expired());
 * @param[in] ms milli-seconds to delay.
 */
#define PROTO_THREAD_DELAY(ms)				\
  do {							\
    set_timer(ms);					\
    PROTO_THREAD_AWAIT(timer_expired());		\
  } while (0)

/**
 * Marks the running thread as TERMINATED and detach from any queue.
 * Should be the last statement in the thread run() function.
 */
#define PROTO_THREAD_END()				\
  do {							\
    ProtoThread::end();					\
    return;						\
  } while (0)

#endif

