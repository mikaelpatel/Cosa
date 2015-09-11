/**
 * @file CosaBlinkInterrupt.ino
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
 * Cosa LED blink with watchdog timeout interrupt callback and sleep
 * mode for low power.
 *
 * @section Circuit
 * This example requires no special circuit. The Arduino Pin 13
 * (built-in LED) is used.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Job.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

// Use the watchdog job scheduler
Watchdog::Scheduler scheduler;

// Interrupt handler; toggle the led for each interrupt
class Blink : public Job, public OutputPin {
public:
  Blink(Job::Scheduler* scheduler, uint16_t ms) :
    Job(scheduler),
    OutputPin(Board::LED),
    m_period(ms)
  {}
  virtual void on_expired()
  {
    toggle();
    schedule();
  }
  void schedule()
  {
    expire_after(m_period);
    start();
  }
  void period(uint16_t ms)
  {
    m_period = ms;
  }

private:
  uint16_t m_period;
};

Blink led(&scheduler, 512);

void setup()
{
  Watchdog::begin();
  led.schedule();
}

void loop()
{
  sleep(10);
  led.period(128);
  sleep(10);
  led.period(256);
  sleep(10);
  led.period(512);
}
