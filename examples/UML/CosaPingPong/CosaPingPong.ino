/**
 * @file CosaPingPong.ino
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
 * Benchmark the Cosa UML Controller and Connector signalling. A
 * simple ping-ping model. The echo capsule will wait for a signal
 * on one port and send a signal on another port directly after
 * receiving a signal. To allow measurement of the controller
 * performance the echoing of signals will also decrement a counter
 * until zero.
 *
 * @section Diagram
 * 
 *     Echo                    Echo
 *  +--------+              +--------+
 *  |  ping  |-----[c1]---->|  pong  |
 *  |        | Echo::Signal |        |
 *  |        |<----[c2]-----|        |
 *  +--------+              +--------+
 *
 * @section Note
 * The measurement includes the test and decrement of the 16-bit
 * counter, reading the input connector, writing the output connector
 * and scheduling the listener. Total 14 us (224 clock cycles).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/UML/Capsule.hh"
#include "Cosa/UML/Connector.hh"
#include "Cosa/UML/Controller.hh"

using namespace UML;

class Echo : public Capsule {
public:
  /**
   * Default number of times to echo.
   */
  static const uint16_t DEFAULT_COUNT = 1000;

  /**
   * Number of times to echo.
   */
  static uint16_t count;

  /**
   * Echo signal connector type.
   */
  typedef Connector<bool> Signal;

  /**
   * Construct echo capsule with given input and output signal
   * connectors. 
   * @param[in] sin input signal connector.
   * @param[in] sout output signal connector.
   */
  Echo(Signal& sin, Signal& sout) :
    Capsule(),
    m_sin(sin),
    m_sout(sout)
  {}

  /**
   * Echo behavior: while counter is not zero, write input signal value
   * output signal, and decrement counter.
   */
  virtual void behavior()
  {
    if (count == 0) return;
    m_sout = m_sin;
    count -= 1;
  }

protected:
  Signal& m_sin;
  Signal& m_sout;
};

// Initial count
uint16_t Echo::count = Echo::DEFAULT_COUNT;

// Forward declaration of the connectors
extern Echo::Signal c1;
extern Echo::Signal c2;

// The capsules with data dependencies (connectors)
Echo ping(c2, c1);
Echo pong(c1, c2);

// The wiring; control dependencies
Capsule* const c1_listeners[] __PROGMEM = { &pong, NULL };
Echo::Signal c1(c1_listeners, true);

Capsule* const c2_listeners[] __PROGMEM = { &ping, NULL};
Echo::Signal c2(c2_listeners, true);

void setup()
{
  // Start trace on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPingPong: started"));

  // Use the watchdog for timeout events
  Watchdog::begin(16, Watchdog::push_timeout_events);

  // Start RTC for measurement
  RTC::begin();
}

void loop()
{
  // Trigger start and measure echo
  c1 = true;
  MEASURE("Run controller:", Echo::DEFAULT_COUNT)
    controller.run();
  ASSERT(true == false);
}
