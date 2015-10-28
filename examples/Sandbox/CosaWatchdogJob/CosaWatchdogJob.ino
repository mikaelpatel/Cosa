/**
 * @file CosaWatchdogJob.ino
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
 * @section Description
 * Demonstration of Cosa Job and Watchdog Job Schedulers.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Job.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

class Work : public Job {
public:
  Work(Job::Scheduler* scheduler, uint32_t delay, Work* chain) :
    Job(scheduler),
    m_delay(delay),
    m_chain(chain),
    m_cycle(0)
  {
    expire_at(delay);
  }

  virtual void run()
  {
    uint32_t now = time();
    uint32_t expire = expire_at();
    int32_t diff = expire - now;
    m_chain->expire_at(expire + m_delay);
    m_chain->start();
    trace << now << ':' << expire
	  << (diff < 0 ? PSTR(":T") : PSTR(":T+")) << diff
	  << PSTR(":work=") << (void*) this
	  << PSTR(",delay=") << m_delay
	  << PSTR(",cycle=") << ++m_cycle
	  << endl;
  }

private:
  uint32_t m_delay;
  Work* m_chain;
  uint16_t m_cycle;
};

// Use the watchdog scheduler (milli-seconds)
Watchdog::Scheduler scheduler;

// Forward declare for cyclic reference
extern Work w0;
extern Work w1;
extern Work w2;
extern Work w3;
extern Work w4;

// Chain
// (w0)->64->(w1)-128ms->(w2)-2048ms->(w3)-16ms->(w0)
Work w0(&scheduler, 64, &w1);
Work w1(&scheduler, 128, &w2);
Work w2(&scheduler, 2048, &w3);
Work w3(&scheduler, 16, &w0);

// Periodic
// (w4)-4096->(w4)
Work w4(&scheduler, 4096, &w4);

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaWatchdogJob: started"));
  trace.flush();

  // Start the work
  w1.start();
  w4.start();

  // Start the watchdog clock
  Watchdog::begin();
}

void loop()
{
  Event::service();
}

