/**
 * @file CosaBenchmarkFSM.ino
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
 * Cosa FSM Benchmark; number of micro-seconds for a send/dispatch
 * cycle. The classical finite state machine benchmark with
 * measurement of event send time. The machine has only one state and
 * for each received event sends an event to a connected machine.
 * The measurement contains the pushing of the event onto the event
 * queue, pulling and dispatch of the event to the receiving state
 * machine.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output,
 * internal timer for RTC and watchdog.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FSM.hh"
#include "Cosa/Memory.h"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

/**
 * Simple echo state machine with a single state.
 */
class Echo : public FSM {
public:
  /**
   * Construct the echo state machine.
   */
  Echo() : FSM(echoState), m_port(0) {}

  /**
   * Bind receiving fsm to port.
   * @param[in] fsm state machine to receive the event.
   */
  void bind(FSM* fsm)
  {
    m_port = fsm;
  }

  /**
   * The state; echoState, while the events count is non zero
   * decrement and send a reply.
   */
  static bool echoState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    Echo* echo = (Echo*) fsm;
    echo->m_port->send(Event::USER_TYPE);
    return (true);
  }

private:
  /** Port (pointer) to receiving state-machine */
  FSM* m_port;
};

// The ping-pong state machines
Echo ping;
Echo pong;

void setup()
{
  // Start the UART and trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkFSM: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Event::Handler));
  TRACE(sizeof(Link));
  TRACE(sizeof(FSM));
  TRACE(sizeof(Echo));

  // Give some more startup info
  TRACE(F_CPU);
  TRACE(I_CPU);

  // Start the watchdog with default 16 ms ticks
  Watchdog::begin();
  RTT::begin();

  // Bind the state machines to each other
  ping.bind(&pong);
  pong.bind(&ping);

  // Send a first event to start the benchmark
  ping.send(Event::USER_TYPE);
}

void loop()
{
  // Dispatch events and measure time per dispatch
  MEASURE("event dispatch: ", 1000) {
    Event event;
    Event::queue.await(&event);
    event.dispatch();
  }

  // Run the loop a limited number of times
  static uint8_t count = 0;
  count += 1;
  ASSERT(count < 10);
}
