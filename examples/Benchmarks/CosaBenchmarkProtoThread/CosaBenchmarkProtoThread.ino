/**
 * @file CosaBenchmarkProtoThread.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * Cosa ProtoThread Benchmark; number of micro-seconds for a thread
 * dispatch and enqueuing in watchdog timer queue.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/ProtoThread.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Counter thread class; two delay periods before incrementing
// counter to show how the internal "instruction pointer" is
// changed at each PROTO_THREAD_AWAIT.
class Counter : public ProtoThread {
private:
  uint16_t m_count;
  uint16_t m_delay;

public:
  Counter(uint16_t delay) : 
    ProtoThread(), 
    m_count(0), 
    m_delay(delay) 
  {}
  
  virtual void run(uint8_t type, uint16_t value)
  {
    DEBUG("thread#%p: ip = %p, count = %d", this, m_ip, m_count);
    PROTO_THREAD_BEGIN();
    PROTO_THREAD_YIELD();
    while (1) {
      PROTO_THREAD_DELAY(m_delay);
      m_count++;
    }
    PROTO_THREAD_END();
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
  trace.begin(&uart, PSTR("CosaBenchmarkProtoThread: started"));
  TRACE(sizeof(ProtoThread));
  TRACE(sizeof(Counter));

  // Print CPU clock and instructions per 1MHZ 
  TRACE(F_CPU);
  TRACE(I_CPU);

  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, Watchdog::push_timeout_events);
  RTC::begin();

  // Start the counter threads
  cnt1.begin();
  cnt2.begin();
  cnt3.begin();
}

void loop()
{
  ProtoThread::dispatch();
  ProtoThread::dispatch();

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
