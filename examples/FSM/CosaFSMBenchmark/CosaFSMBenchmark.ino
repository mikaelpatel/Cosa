/**
 * @file CosaFSMBenchmark.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * cycle.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FSM.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

/**
 * Number of messages to send in the benchmark.
 */
const uint32_t EVENTS_MAX = 100000L;

/**
 * Simple echo state machine with a single state. 
 */
class Echo : public FSM {

private:
  FSM* m_port;

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
    return (1);
  }
};

// The ping-pong state machines
Echo ping;
Echo pong;

void setup()
{
  // Start the trace output stream
  trace.begin(9600, PSTR("CosaFSMBenchmark: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(FSM));
  TRACE(sizeof(Echo));

  // Start the watchdog in default 16 ms ticks
  Watchdog::begin();

  // Bind the state machines to each other
  ping.bind(&pong);
  pong.bind(&ping);

  // Give some more startup info
  TRACE(F_CPU);
  TRACE(I_CPU);
  TRACE(EVENTS_MAX);
}

void loop()
{
  // Initiate the counters and reset the watchdog ticks for measurement.
  // Note the resolution is only 16 ms ticks by default.
  uint32_t events = EVENTS_MAX;
  Watchdog::reset();

  // Send a first event to start the benchmark and dispatch events.
  ping.send(Event::USER_TYPE);
  while (events--) {
    Event event;
    Event::queue.await(&event);
    event.dispatch();
  }

  // Capture the tick count and calculate the time per event and nr of cycles.
  uint16_t ticks = Watchdog::get_ticks();
  uint16_t ms_per_tick = Watchdog::ms_per_tick();
  uint32_t ms = ticks * ms_per_tick;
  uint32_t us_per_event = (ms * 1000L) / EVENTS_MAX;
  INFO("%l us per event (%l cycles)", us_per_event, us_per_event * I_CPU);
}
