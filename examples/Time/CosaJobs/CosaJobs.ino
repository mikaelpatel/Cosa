/**
 * @file CosaJobs.ino
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
 * Demonstrate Hierarchical Periodic functions. Periodic LED class
 * will toggle pin for a given number of steps and stop. A Periodic
 * Controller class is used to start the LED. The LED class uses
 * on_expired() for the behavior, while the Controller uses an
 * event and the run() member function.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Event.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/OutputPin.hh"

// Configuration; trace and timer source (RTT/Watchdog)
#define USE_TRACE_CONTROLLER
#define USE_RTT_TIMER
// #define USE_WATCHDOG_TIMER

#if defined(USE_TRACE_CONTROLLER)
#include "Cosa/UART.hh"
#include "Cosa/Trace.hh"
#endif

#if defined(USE_RTT_TIMER)
#include "Cosa/RTT.hh"
#define TIMER RTT
#define TIMESTAMP() micros() / 1000
#define MS(ms) ms * 1000L
#endif

#if defined(USE_WATCHDOG_TIMER)
#include "Cosa/Watchdog.hh"
#define TIMER Watchdog
#define TIMESTAMP() millis()
#define MS(ms) ms
#endif

class LED : public Periodic, public OutputPin {
public:
  const uint16_t STEP_MAX;

  LED(Job::Scheduler* scheduler, uint32_t period, uint16_t steps) :
    Periodic(scheduler, period),
    OutputPin(Board::LED, 0),
    STEP_MAX(steps),
    m_step(0),
    m_cycles(0)
  {}

  virtual void on_expired()
  {
    if (m_step < STEP_MAX) {
      toggle();
      m_cycles += 1;
      m_step += 1;
      reschedule();
    }
    else {
      off();
      m_step = 0;
    }
  }

  uint32_t cycles() const
  {
    return (m_cycles);
  }

private:
  uint16_t m_step;
  uint32_t m_cycles;
};

class Controller : public Periodic {
public:
  Controller(Job::Scheduler* scheduler, uint32_t period, LED* led) :
    Periodic(scheduler, period),
    m_led(led)
  {}

  virtual void run()
  {
#if defined(USE_TRACE_CONTROLLER)
    uint32_t timestamp = TIMER::TIMESTAMP();
    uint16_t cycles = m_led->cycles();
#endif
    m_led->expire_at(expire_at());
    m_led->start();
#if defined(USE_TRACE_CONTROLLER)
    trace << timestamp << ':' << cycles << endl;
#endif
  }

private:
  LED* m_led;
};

TIMER::Scheduler scheduler;
LED led(&scheduler, MS(200L), 10);
Controller controller(&scheduler, MS(5000L), &led);

void setup()
{
#if defined(USE_TRACE_CONTROLLER)
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaJobs: started"));
  trace.flush();
#endif
  controller.start();
  TIMER::begin();
}

void loop()
{
  Event::service();
}
