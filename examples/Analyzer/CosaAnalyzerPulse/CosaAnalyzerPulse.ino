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
 * Logic Analyzer based analysis of Job with RTC Scheduler.
 *
 * @section Circuit
 * Trigger on CHAN0/D8 rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> D8
 * | CHAN1 |-------------------------------> D9
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Job.hh"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Select call from interrupt service routine or event handler. Latency:
// Interrupt Service Routine: 1-5 us
// Event Handler: 20-25 us
#define USE_ISR_DISPATCH

class Work : public Job {
public:
  Work(Job::Scheduler* scheduler,
       Board::DigitalPin pin, uint32_t delay,
       Work& chain) :
    Job(scheduler),
    m_pin(pin),
    m_delay(delay),
    m_chain(chain)
  {
    expire_at(delay);
  }

#if defined(USE_ISR_DISPATCH)
  virtual void on_expired()
  {
    run();
  }
#endif

  virtual void run()
  {
    m_pin.toggle();
    m_chain.expire_after(m_delay);
    m_chain.start();
    m_pin.toggle();
  }

private:
  OutputPin m_pin;
  uint32_t m_delay;
  Work& m_chain;
};

// Use the real-time clock scheduler (micro-seconds)
RTC::Scheduler scheduler;

// Forward declare for cyclic reference
extern Work w0;
extern Work w1;
extern Work w2;
extern Work w3;
extern Work w4;

// Periodic
// (w0)-200ms->(w0)
Work w0(&scheduler, Board::D8, 200000UL, w0);

// Chain
// (w1)-150us->(w2)-400us->(w3)-1200us->(w4)-250us->(w1)
Work w1(&scheduler, Board::D9,    150UL, w2);
Work w2(&scheduler, Board::D9,    400UL, w3);
Work w3(&scheduler, Board::D9,   1200UL, w4);
Work w4(&scheduler, Board::D9,    250UL, w1);

void setup()
{
  // Print Info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerJob: started"));
  trace << PSTR("CHAN0 - D8 [^]") << endl;
  trace << PSTR("CHAN1 - D9") << endl;
  trace.flush();

  // Start the real-time clock and scheduler
  RTC::begin();
  RTC::job(&scheduler);

  // Start the work
  w0.start();
  w4.start();
}

void loop()
{
  Event::service();
}

