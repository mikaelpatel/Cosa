/**
 * @file CosaAnalyzerServo.ino
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
 * Logic Analyzer based analysis of Job Scheduler; generate pulse
 * for servo.
 *
 * @section Circuit
 * Trigger on CHAN0/D13 rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> D13
 * | CHAN1 |-------------------------------> D12
 * | ..... |
 * | CHAN7 |-------------------------------> D0/TX
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Job.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/RTT.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Call directly from interrupt service routine
#define USE_ISR_DISPATCH
// #define USE_VOLTMETER_CONTROLLER
#define USE_CLOCK_CONTROLLER

class Servo : public Job {
public:
  static const uint32_t PERIOD = 20000UL;
  static const uint32_t MIN_WIDTH = 700UL;
  static const uint32_t MAX_WIDTH = 2100UL;
  static const uint32_t WIDTH = (MAX_WIDTH - MIN_WIDTH);
  Servo(Job::Scheduler* scheduler, Board::DigitalPin pin) :
    Job(scheduler),
    m_pin(pin)
  {
    pulse(0);
    expire_at(PERIOD);
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
    if (m_pin.is_set()) {
      expire_after(m_width);
    }
    else
      expire_after(PERIOD - m_width);
    start();
  }

  uint32_t pulse() const
  {
    return (m_width);
  }

  void pulse(int angle)
  {
    synchronized {
      m_width = MIN_WIDTH + (((WIDTH * (uint32_t) angle) + 90) / 180);
      if (m_width < MIN_WIDTH) m_width = MIN_WIDTH;
      else if (m_width > MAX_WIDTH) m_width = MAX_WIDTH;
    }
  }

private:
  OutputPin m_pin;
  uint32_t m_width;
};

class Controller : public Periodic {
public:
  Controller(Job::Scheduler* scheduler, Servo* servo, int incr) :
    Periodic(scheduler, 1),
    m_probe(Board::A0),
    m_led(Board::LED),
    m_servo(servo),
    m_incr(incr),
    m_angle(0),
    m_delta(incr)
  {
  }

  virtual void run()
  {
    m_led.toggle();
#if defined(USE_VOLTMETER_CONTROLLER)
    m_angle = (m_probe * 180UL) / 1023;
    m_servo->pulse(m_angle);
#endif
    trace << m_angle << endl;
#if defined(USE_CLOCK_CONTROLLER)
    m_servo->pulse(m_angle);
    if (m_angle == 180) m_delta = -m_incr;
    if (m_angle == 0) m_delta = m_incr;
    m_angle += m_delta;
#endif
  }

private:
  AnalogPin m_probe;
  OutputPin m_led;
  Servo* m_servo;
  uint8_t m_incr;
  uint8_t m_angle;
  int8_t m_delta;
};

// The job schedulers; micro-seconds and seconds level
RTT::Scheduler scheduler;
RTT::Clock clock;

// Servo and Controller
Servo s1(&scheduler, Board::D12);
Controller c1(&clock, &s1, 9);

void setup()
{
  // Print Info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerServo: started"));
  trace << PSTR("CHAN0 - D13 (") << c1.period() << PSTR(" s)") << endl;
  trace << PSTR("CHAN1 - D12 (") << s1.pulse() << PSTR(" us)") << endl;
  trace << PSTR("RTT Job Scheduler and Clock") << endl;
#if defined(USE_ISR_DISPATCH)
  trace << PSTR("Servo ISR dispatch") << endl;
#else
  trace << PSTR("Servo Event dispatch") << endl;
#endif
  trace << PSTR("Controller Event dispatch") << endl;
#if defined(USE_VOLTMETER_CONTROLLER)
  trace << PSTR("Voltmeter Controller") << endl;
#endif
#if defined(USE_CLOCK_CONTROLLER)
  trace << PSTR("Clock Controller") << endl;
#endif
  trace.flush();

  // Start the servo pulse width generators
  s1.start();
  c1.start();

  // Start the timer
  RTT::begin();
}

void loop()
{
  Event::service();
}

