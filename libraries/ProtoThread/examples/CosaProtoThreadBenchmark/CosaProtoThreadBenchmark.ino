/**
 * @file CosaBenchmarkProtoThread.ino
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
 * @section Description
 * Cosa ProtoThread Benchmark; number of micro-seconds for a thread
 * dispatch and enqueuing in watchdog timer queue.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <ProtoThread.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Counter thread class; two delay periods before incrementing
// counter to show how the internal "instruction pointer" is
// changed at each PROTO_THREAD_AWAIT.
class Counter : public ProtoThread {
public:
  Counter(Job::Scheduler* scheduler, uint16_t delay) :
    ProtoThread(scheduler),
    m_count(0),
    m_delay(delay)
  {}

  virtual void on_run(uint8_t type, uint16_t value)
  {
    UNUSED(type);
    UNUSED(value);
    DEBUG("thread#%p: ip = %p, count = %d", this, m_ip, m_count);
    PROTO_THREAD_BEGIN();
    PROTO_THREAD_YIELD();
    while (1) {
      PROTO_THREAD_DELAY(m_delay);
      m_count++;
    }
    PROTO_THREAD_END();
  }

private:
  uint16_t m_count;
  uint16_t m_delay;
};

// Use the watchdog job scheduler
Watchdog::Scheduler scheduler;

// Three counter threads with different delay periods
Counter cnt1(&scheduler, 32);
Counter cnt2(&scheduler, 128);
Counter cnt3(&scheduler, 512);

void setup()
{
  // Start the UART and trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkProtoThread: started"));
  TRACE(sizeof(Job));
  TRACE(sizeof(ProtoThread));
  TRACE(sizeof(Counter));

  // Print CPU clock and instructions per 1MHZ
  TRACE(F_CPU);
  TRACE(I_CPU);

  // Start the watchdog and rtc
  Watchdog::begin();
  RTT::begin();

  // Start the counter threads
  cnt1.begin();
  cnt2.begin();
  cnt3.begin();
}

void loop()
{
  ProtoThread::dispatch();
  ProtoThread::dispatch();

  // Dispatch events and measure time per dispatch
  MEASURE("event dispatch: ", 100) {
    Event event;
    Event::queue.await(&event);
    event.dispatch();
  }

  // Run the loop a limited number of times
  static uint8_t count = 0;
  count += 1;
  ASSERT(count < 10);
}
