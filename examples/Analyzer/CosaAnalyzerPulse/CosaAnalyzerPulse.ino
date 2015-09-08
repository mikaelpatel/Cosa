/**
 * @file CosaAnalyzerPulse.ino
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
 * Logic Analyzer based analysis of Job with RTC Scheduler; generate
 * 100 ms pulse width [10..90%].
 *
 * @section Circuit
 * Trigger on CHAN0/D8 rising.
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

#include "Cosa/Job.hh"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class Pulse : public Job {
public:
  static const uint32_t START = 1000000UL;
  static const uint32_t WIDTH = 100000UL;

  Pulse(Job::Scheduler* scheduler, uint16_t percent,
	Board::DigitalPin pin) :
    Job(scheduler),
    m_pin(pin)
  {
    expire_at(START);
    width(percent);
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
    if (m_pin.is_set())
      expire_after(m_pulse);
    else
      expire_after(WIDTH - m_pulse);
    start();
  }

  void width(uint16_t percent)
  {
    if (percent < 10) percent = 10;
    else if (percent > 90) percent = 90;
    m_pulse = (WIDTH * percent) / 100UL;
  }

private:
  OutputPin m_pin;
  uint32_t m_pulse;
};

// Use the real-time clock scheduler (micro-seconds)
RTC::Scheduler scheduler;

// Pulse generators with 10, 50 and 90% pulse width
Pulse p1(&scheduler, 10, Board::D8);
Pulse p2(&scheduler, 50, Board::D9);
Pulse p3(&scheduler, 90, Board::D10);

void setup()
{
  // Print Info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerPulse: started"));
  trace << PSTR("CHAN0 - D8 [^]") << endl;
  trace << PSTR("CHAN1 - D9") << endl;
  trace << PSTR("CHAN2 - D10") << endl;
  trace.flush();

  // Start the real-time clock and scheduler
  RTC::begin();
  RTC::job(&scheduler);

  // Start the pulse width generators
  p1.start();
  p2.start();
  p3.start();
}

void loop()
{
  Event::service();
}

