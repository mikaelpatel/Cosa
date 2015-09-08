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
 * Logic Analyzer based analysis of Periodic Job with RTC Scheduler.
 *
 * @section Circuit
 * Trigger on CHAN0/D13/LED rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> D8
 * | CHAN1 |-------------------------------> D9
 * | CHAN2 |-------------------------------> D10
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

// Configuration
#define USE_RTC
// #define USE_WATCHDOG
#define USE_ISR_DISPATCH

#if defined(USE_RTC)
#define TIMER RTC
#endif

#if defined(USE_WATCHDOG)
#define TIMER Watchdog
#endif

class Work : public Periodic {
public:
  Work(Job::Scheduler* scheduler, uint32_t delay, Board::DigitalPin pin) :
    Periodic(scheduler, delay),
    m_pin(pin)
  {
  }

#if defined(USE_ISR_DISPATCH)
  virtual void on_expired()
  {
    run();
    start();
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

// Use the timer scheduler
TIMER::Scheduler scheduler;

// Periodic work
#if defined(USE_RTC)
Work w1(&scheduler, 1000, Board::D8);
Work w2(&scheduler, 500, Board::D9);
Work w3(&scheduler, 250, Board::D10);
#endif

#if defined(USE_WATCHDOG)
Work w1(&scheduler, 64, Board::D8);
Work w2(&scheduler, 32, Board::D9);
Work w3(&scheduler, 16, Board::D10);
#endif

void setup()
{
  // Print info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerPeriodic: started"));
  trace << PSTR("CHAN0 - D8 [^]") << endl;
  trace << PSTR("CHAN1 - D9") << endl;
  trace << PSTR("CHAN2 - D10") << endl;
  trace.flush();

  // Start the timer and scheduler
  TIMER::begin();
  TIMER::job(&scheduler);

  // Start the work
  w1.start();
  w2.start();
  w3.start();
}

void loop()
{
  Event::service();
}

