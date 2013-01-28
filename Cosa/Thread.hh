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
#include "Cosa/Watchdog.hh"

class Thread : public Link {
protected:
  uint16_t m_param;
  uint8_t m_type;
  uint8_t m_timer;
  void* m_ip;

  /**
   * @override
   * The first level event handler. Filters timeout events and
   * run the thread action function.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    if (m_timer) {
      m_timer = 0;
      detach();
    }
    m_type = type;
    m_param = value;
    run();
  }
  
public:
  /**
   * Construct thread and initial.
   */
  Thread() : 
    Link(), 
    m_timer(0)
  {}
  
  /**
   * Get event type.
   * @return event type.
   */
  uint8_t get_event_type()
  {
    return (m_type);
  }
  
  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get_event_param(uint16_t& param)
  {
    param = m_param;
  }
  
  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get_event_param(void*& param)
  {
    param = (void*) m_param;
  }
  
  /**
   * Start the thread.
   */
  bool begin()
  {
    m_ip = 0;
    m_timer = 0;
    run();
    return (1);
  }
  
  /**
   * End the thread.
   */
  void end()
  {
    if (m_timer) detach();
  }
  
  /**
   * Set timer for time out events.
   * @param[in] ms timeout period.
   */
  void set_timer(uint16_t ms)
  {
    m_timer = 1;
    m_type = 0;
    Watchdog::attach(this, ms);
  }

  /**
   * Check if timer expired. 
   */
  bool timer_expired()
  {
    return (m_type == Event::TIMEOUT_TYPE);
  }

  /**
   * Thread activity. Must be overridden. 
   */
  virtual void run() = 0;
};

/**
 * Thread action function support macros.
 */
#define THREAD_BEGIN() if (m_ip != 0) goto *m_ip
#define THREAD_AWAIT(cond) UNIQUE(L): if (!(cond)) { m_ip = &&UNIQUE(L); return; }
#define THREAD_END() THREAD_AWAIT(0)

#endif

