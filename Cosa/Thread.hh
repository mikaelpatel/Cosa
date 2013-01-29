/**
 * @file Cosa/Thread.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * Cosa implementation of protothreads; A protothread is a
 * low-overhead mechanism for concurrent programming.  Protothreads
 * function as stackless, lightweight threads providing a blocking
 * context cheaply using minimal memory per protothread (on the order
 * of single bytes). http://en.wikipedia.org/wiki/Protothreads
 *
 * @section Limitations
 * The thread macro set should only be used within the Thread::run()
 * function. 
 *
 * @section Acknowledgements
 * Inspired by research and prototype by Adam Dunkel and the
 * protothread library by Larry Ruane.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_THREAD_HH__
#define __COSA_THREAD_HH__

#include "Cosa/Event.hh"
#include "Cosa/Linkage.hh"

class Thread : public Link {
public:
  enum {
    INIT = 0,
    READY,
    WAITING,
    TIMEOUT,
    SLEEPING,
    RUNNING,
    TERMINATED = 0xff,
  };

protected:
  static Head runq;
  uint8_t m_state;
  void* m_ip;

  /**
   * @override
   * The first level event handler. Filters timeout events and
   * run the thread action function.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
  
public:
  /**
   * Construct thread and initial.
   */
  Thread() : 
    Link(),
    m_state(INIT),
    m_ip(0)
  {}
  
  /**
   * Start the thread. Must be in INIT state to be allowed to be
   * scheduled.
   * @return bool true if scheduled otherwise false.
   */
  bool begin()
  {
    if (m_state != INIT) return (0);
    schedule(this);
    return (1);
  }
  
  /**
   * End the thread. Mark as terminated and remove from any queue.
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
  uint8_t get_state()
  {
    return (m_state);
  }

  /**
   * Set timer for time out events.
   * @param[in] ms timeout period.
   */
  void set_timer(uint16_t ms);

  /**
   * Check if timer expired. 
   */
  bool timer_expired()
  {
    return (m_state == TIMEOUT);
  }

  /**
   * Thread activity. Must be overridden. 
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void run(uint8_t type = Event::NULL_TYPE, uint16_t value = 0) = 0;

  /**
   * Thread dispatch. Run threads in the run queue. If given flag is true
   * events will be processes. Returns number of dispatched threads and
   * events.
   * @param[in] flag process event.
   * @return number of dispatched threads and events.
   */
  static uint16_t dispatch(uint8_t flag = 0);

  /**
   * Thread schedule. Add the given thread to the run queue.
   * @param[in] thread 
   */
  static void schedule(Thread* thread)
  {
    thread->m_state = READY;
    runq.attach(thread);
  }
};

/**
 * Thread action function support macros.
 */
#define THREAD_BEGIN()					\
  if (m_ip != 0) goto *m_ip

#define THREAD_YIELD() \
  do {							\
    m_ip = &&UNIQUE(LABEL);				\
    return;						\
    UNIQUE(LABEL):					\
      ;							\
  } while (0)

#define THREAD_SLEEP()					\
  do {							\
    m_state = SLEEPING;					\
    detach();						\
    THREAD_YIELD();					\
  } while (0)

#define THREAD_WAKE(thread)				\
  do {							\
    if (thread->m_state == SLEEPING)			\
      Thread::schedule(thread);				\
  } while (0)

#define THREAD_AWAIT(condition)				\
  do {							\
  UNIQUE(LABEL):					\
    if (!(condition)) {					\
      m_ip = &&UNIQUE(LABEL);				\
      return;						\
    }							\
  } while (0)

#define THREAD_END()					\
  Thread::end()

#endif

