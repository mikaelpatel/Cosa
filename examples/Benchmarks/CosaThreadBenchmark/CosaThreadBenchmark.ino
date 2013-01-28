/**
 * @file CosaThreadBenchmark.ino
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
 * Cosa Thread Benchmark; number of micro-seconds for a thread
 * dispatch/timer.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Thread.hh"
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
  
  virtual void run()
  {
    INFO("thread#%p: ip = %p, count = %d", this, m_ip, m_count);
    THREAD_BEGIN();
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
Counter cnt1(512);
Counter cnt2(1024);
Counter cnt3(2048);

void setup()
{
  // Start the UART and trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaThreadBenchmark: started"));
  TRACE(sizeof(Thread));
  TRACE(sizeof(Counter));

  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Start the counter threads
  cnt1.begin();
  cnt2.begin();
  cnt3.begin();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
