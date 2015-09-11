/**
 * @file CosaAnalyzerPeriodic.ino
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
 * Logic Analyzer based analysis of Periodic jobs.
 *
 * @section Circuit
 * Trigger on CHAN0/D7 rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> D7
 * | CHAN1 |-------------------------------> D8
 * | CHAN2 |-------------------------------> D9
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Periodic.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Use the RTC or Watchdog Job Scheduler
#define USE_RTC
// #define USE_WATCHDOG

// Call directly from interrupt
// #define USE_ISR_DISPATCH

#if defined(USE_RTC)
#define TIMER RTC
#define SCALE(x) (x) * 1000UL
#endif

#if defined(USE_WATCHDOG)
#define TIMER Watchdog
#define SCALE(x) (x)
#endif

class Work : public Periodic {
public:
  const uint32_t START = SCALE(1000);

  Work(Job::Scheduler* scheduler, uint32_t delay, Board::DigitalPin pin) :
    Periodic(scheduler, delay),
    m_pin(pin)
  {
    expire_at(START);
  }

#if defined(USE_ISR_DISPATCH)
  // If called direclty from the interrupt the periodic job will
  // need to be rescheduled
  virtual void on_expired()
  {
    run();
    reschedule();
  }
#endif

  virtual void run()
  {
    m_pin.toggle();
    DELAY(10);
    m_pin.toggle();
  }

private:
  OutputPin m_pin;
};

// The timer scheduler
TIMER::Scheduler scheduler;

// The periodic work
Work w1(&scheduler, SCALE(640), Board::D7);
Work w2(&scheduler, SCALE(320), Board::D8);
Work w3(&scheduler, SCALE(160), Board::D9);

void setup()
{
  // Print info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerPeriodic: started"));
  trace << PSTR("CHAN0 - D7 [^]") << endl;
  trace << PSTR("CHAN1 - D8") << endl;
  trace << PSTR("CHAN2 - D9") << endl;
#if defined(USE_RTC)
  trace << PSTR("RTC Job Scheduler") << endl;
#endif
#if defined(USE_WATCHDOG)
  trace << PSTR("Watchdog Job Scheduler") << endl;
#endif
#if defined(USE_ISR_DISPATCH)
  trace << PSTR("ISR dispatch") << endl;
#else
  trace << PSTR("Event dispatch") << endl;
#endif
  trace.flush();

  // Start the work
  w1.start();
  w2.start();
  w3.start();

  // Start the timer
  TIMER::begin();
}

void loop()
{
  Event::service();
}

