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
 * Trigger on CHAN0/D13 rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> D13
 * | CHAN1 |-------------------------------> D12
 * | CHAN2 |-------------------------------> D11
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Periodic.hh"
#include "Cosa/Power.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Use the RTT or Watchdog Job Scheduler
#define USE_RTT
// #define USE_WATCHDOG

// Call directly from interrupt and use one of the scheduling types
// #define USE_ISR_TIME_PERIOD
#define USE_ISR_RESCHEDULE

// Use low power sleep mode
#define USE_LOW_POWER

#if defined(USE_RTT)
#define TIMER RTT
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

#if defined(USE_ISR_TIME_PERIOD)
  virtual void on_expired()
  {
    run();
    expire_at(time() + period());
    start();
  }
#endif

#if defined(USE_ISR_RESCHEDULE)
  virtual void on_expired()
  {
    run();
    reschedule();
  }
#endif

  virtual void run()
  {
    m_pin.toggle();
    DELAY(50);
    m_pin.toggle();
  }

private:
  OutputPin m_pin;
};

// The timer scheduler
TIMER::Scheduler scheduler;

// The periodic work
Work w1(&scheduler, SCALE(640), Board::D13);
Work w2(&scheduler, SCALE(320), Board::D12);
Work w3(&scheduler, SCALE(160), Board::D11);

void setup()
{
  // Print info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerPeriodic: started"));
  trace << PSTR("CHAN0 - D13 [^]") << endl;
  trace << PSTR("CHAN1 - D12") << endl;
  trace << PSTR("CHAN2 - D11") << endl;
#if defined(USE_RTT)
  trace << PSTR("RTT Job Scheduler") << endl;
#endif
#if defined(USE_WATCHDOG)
  trace << PSTR("Watchdog Job Scheduler") << endl;
#endif
#if defined(USE_ISR_RESCHEDULE)
  trace << PSTR("ISR with reschedule") << endl;
#elif defined(USE_ISR_TIME_PERIOD)
  trace << PSTR("ISR with time period") << endl;
#else
  trace << PSTR("Event dispatch") << endl;
#endif

  // Start the work
  w1.start();
  w2.start();
  w3.start();

  // Set low power mode (Watchdog: 7.8/0.3 mA, RTT: 8.2/1.75 mA)
#if defined(USE_LOW_POWER)
# if defined(USE_RTT)
  trace << PSTR("Extended Standby") << endl;
  trace.flush();
  Power::set(SLEEP_MODE_EXT_STANDBY);
# endif
# if defined(USE_WATCHDOG)
  trace << PSTR("Power-down") << endl;
  trace.flush();
  Power::set(SLEEP_MODE_PWR_DOWN);
# endif
#endif

  // Start the timer
  TIMER::begin();
}

void loop()
{
  Event::service();
}
