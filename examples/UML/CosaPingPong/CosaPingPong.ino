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
 *         Echo                Echo                Echo
 *      +--------+          +--------+          +--------+
 *      |  ping  |          |  pong  |          |  pang  |
 *  +-->|        |---[c1]-->|        |---[c2]-->|        |---+
 *  |   |        |          |        |          |        |   |
 *  |   +--------+          +--------+          +--------+   |
 *  |                                                        |
 *  +--------------------------[c3]--------------------------+
 *
 * @section Note
 * The measurement includes the test and decrement of the 16-bit
 * counter, reading the input connector, writing the output connector
 * and scheduling the listener. Total 13 us (208 clock cycles).
 * The inner signal port read/write and scheduling of the listener
 * takes 8 us (128 clock cycles).
 *
 * This file is part of the Arduino Che Cosa project.
 */

//#define TRACE_NO_VERBOSE
//#define TRACE_FOOTPRINT

#ifdef TRACE_FOOTPRINT
#include "Cosa/Memory.h"
#include "Cosa/IOBuffer.hh"
#endif

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

#include "Cosa/UML.hh"
#include "Cosa/UML/Capsule.hh"
#include "Cosa/UML/Connector.hh"

using namespace UML;

class Echo : public Capsule {
public:
  /**
   * Default number of times to echo.
   */
  static const uint16_t DEFAULT_COUNT = 10000;

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
extern Echo::Signal c3;

// The capsules with data dependencies (connectors)
Echo ping(c3, c1);
Echo pong(c1, c2);
Echo pang(c2, c3);

// The wiring; control dependencies
Capsule* const c1_listeners[] __PROGMEM = { &pong, NULL };
Echo::Signal c1(c1_listeners, true);

Capsule* const c2_listeners[] __PROGMEM = { &pang, NULL};
Echo::Signal c2(c2_listeners, true);

Capsule* const c3_listeners[] __PROGMEM = { &ping, NULL};
Echo::Signal c3(c3_listeners, true);

void setup()
{
  // Start trace on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPingPong: started"));

#ifdef TRACE_FOOTPRINT
  // Trace available memory and size of data
  TRACE(free_memory());
  TRACE(sizeof(uart));
  TRACE(sizeof(trace));
  TRACE(sizeof(IOBuffer<64>));
  TRACE(sizeof(controller));
  TRACE(sizeof(ping));
  TRACE(sizeof(pong));
  TRACE(sizeof(c1));
  TRACE(sizeof(c2));
#endif

  // Start UML run-time
  UML::begin();
}

void loop()
{
  // Trigger start and measure echo
  Echo::count = Echo::DEFAULT_COUNT;
  c1 = true;
  MEASURE("Run controller:", Echo::DEFAULT_COUNT)
    controller.run();

  // Take a nap
  sleep(3);
}
