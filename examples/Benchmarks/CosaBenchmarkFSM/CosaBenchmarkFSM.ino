/**
 * @file CosaBenchmarkFSM.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
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
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

/**
 * Number of messages to send in the benchmark.
 */
const uint32_t EVENTS_MAX = 100000L;

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
  TRACE(EVENTS_MAX);
  TRACE(F_CPU);
  TRACE(I_CPU);

  // Start the watchdog with default 16 ms ticks
  Watchdog::begin();
  RTC::begin();

  // Bind the state machines to each other
  ping.bind(&pong);
  pong.bind(&ping);

  // Send a first event to start the benchmark
  ping.send(Event::USER_TYPE);
}

void loop()
{
  // Initiate the counters
  uint32_t events = EVENTS_MAX;
  uint32_t start = RTC::micros();

  // Dispatch events and measure total time
  while (events--) {
    Event event;
    Event::queue.await(&event);
    event.dispatch();
  }
  uint32_t stop = RTC::micros();

  // Capture the tick count and calculate the time per event and cycles
  uint32_t us = stop - start;
  uint32_t us_per_event = us / EVENTS_MAX;
  INFO("%l us per event (%l cycles)", us_per_event, us_per_event * I_CPU);
}

/**
@section Output
CosaBenchmarkFSM: started
free_memory() = 1576
sizeof(Event::Handler) = 2
sizeof(Link) = 6
sizeof(FSM) = 12
sizeof(Echo) = 14
EVENTS_MAX = 100000
F_CPU = 16000000
I_CPU = 16
139:void loop():info:13 us per event (208 cycles)
*/
