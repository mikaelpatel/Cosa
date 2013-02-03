/**
 * @file CosaBenchmarkThread.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * Cosa Thread Benchmark; number of micro-seconds for a thread
 * dispatch and enqueuing in watchdog timer queue.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Thread.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Counter thread class; two delay periods before incrementing
// counter to show how the internal "instruction pointer" is
// changed at each THREAD_AWAIT.
class Counter : public Thread {
private:
  uint16_t m_count;
  uint16_t m_delay;

public:
  Counter(uint16_t delay) : 
    Thread(), 
    m_count(0), 
    m_delay(delay) 
  {}
  
  virtual void run(uint8_t type, uint16_t value)
  {
    DEBUG("thread#%p: ip = %p, count = %d", this, m_ip, m_count);
    THREAD_BEGIN();
    THREAD_YIELD();
    while (1) {
      set_timer(m_delay);
      THREAD_AWAIT(timer_expired());
      m_count++;
      set_timer(m_delay);
      THREAD_AWAIT(timer_expired());
      m_count++;
    }
    THREAD_END();
  }
};

// Three counter threads with different delay periods
Counter cnt1(32);
Counter cnt2(128);
Counter cnt3(512);

void setup()
{
  // Start the UART and trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkThread: started"));
  TRACE(sizeof(Thread));
  TRACE(sizeof(Counter));

  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  RTC::begin();

  // Start the counter threads
  cnt1.begin();
  cnt2.begin();
  cnt3.begin();
}

void loop()
{
  Thread::dispatch();
  Thread::dispatch();

  static const uint16_t EVENTS_MAX = 100;
  uint16_t events = EVENTS_MAX;
  uint32_t us = 0;
  Event event;

  while (events--) {
    Event::queue.await(&event);
    uint32_t start = RTC::micros();
    event.dispatch();
    uint32_t stop = RTC::micros();
    us += (stop - start);
  }

  uint32_t us_per_dispatch = us / EVENTS_MAX;
  INFO("%l us per dispatch (%l cycles)", us_per_dispatch, us_per_dispatch * I_CPU);
}
