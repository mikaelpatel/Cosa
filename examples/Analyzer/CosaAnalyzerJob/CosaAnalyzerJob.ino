/**
 * @file CosaAnalyzerJob.ino
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
 * Logic Analyzer based analysis of Job chain scheduling.
 *
 * @section Circuit
 * Trigger on CHAN0/D13 rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> D13
 * | CHAN1 |-------------------------------> D12
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Job.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Power.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Call directly from interrupt
#define USE_ISR_DISPATCH

class Work : public Job {
public:
  Work(Job::Scheduler* scheduler,
       Board::DigitalPin pin,
       uint32_t delay, Work* chain) :
    Job(scheduler),
    m_pin(pin),
    m_delay(delay),
    m_chain(chain)
  {
  }

#if defined(USE_ISR_DISPATCH)
  virtual void on_expired()
  {
    run();
  }
#endif

  // Generate a pulse and schedule the next job in the chain relative
  // to the expire time of the current work
  virtual void run()
  {
    m_pin.toggle();
    m_chain->expire_at(expire_at() + m_delay);
    m_chain->start();
    m_pin.toggle();
  }

private:
  OutputPin m_pin;
  uint32_t m_delay;
  Work* m_chain;
};

// Use the real-time job scheduler (micro-seconds)
RTT::Scheduler scheduler;

// Forward declare for cyclic reference
extern Work w0;
extern Work w1;
extern Work w2;
extern Work w3;
extern Work w4;

// Periodic
// (w0)-200ms->(w0)
Work w0(&scheduler, Board::D13, 200000UL, &w0);

// Chain
// (w1)-150us->(w2)-500us->(w3)-1500us->(w4)-250us->(w1)
Work w1(&scheduler, Board::D12, 150UL, &w2);
Work w2(&scheduler, Board::D12, 500UL, &w3);
Work w3(&scheduler, Board::D12, 1500UL, &w4);
Work w4(&scheduler, Board::D12, 250UL, &w1);

void setup()
{
  // Print Info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerJob: started"));
  trace << PSTR("CHAN0 - D13 [^]") << endl;
  trace << PSTR("CHAN1 - D12") << endl;
  trace << PSTR("RTT Job Scheduler") << endl;
#if defined(USE_ISR_DISPATCH)
  trace << PSTR("ISR dispatch") << endl;
#else
  trace << PSTR("Event dispatch") << endl;
#endif
  trace.flush();

  // Start the work
  const uint32_t START = 1000000UL;
  w0.expire_at(START);
  w1.expire_at(START);
  w0.start();
  w1.start();

  // Use timer based low power sleep (9.2/3.2 mA)
  RTT::begin();
  Power::set(SLEEP_MODE_EXT_STANDBY);
}

void loop()
{
  Event::service();
}

