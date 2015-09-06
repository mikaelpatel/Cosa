/**
 * @file CosaPingPong.ino
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
 * Cosa FSM example with three interacting finite state machines.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FSM.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

/**
 * Simple echo state machine: Listens for an event. When received
 * steps to echo state and on a timeout prints its name and sends an
 * event before stepping back to listen state.
 */
class Echo : public FSM {

private:
  str_P m_name;
  uint16_t m_count;
  FSM* m_port;

public:
  /**
   * Construct the echo state machine. Name and port must be bound
   * before started.
   */
  Echo() : FSM(initState), m_name(0), m_count(0), m_port(0) {}

  /**
   * Bind name and port. The name is used for the trace print
   * and the state machine will receive the echo event.
   * @param[in] name string in program memory.
   * @param[in] fsm state machine to receive the event.
   */
  void bind(str_P name, FSM* fsm)
  {
    m_name = name;
    m_port = fsm;
  }

  /**
   * The states; init, listen and echo.
   * init -> listen: print init message
   * listen -> echo on timeout(0.5 s): print name
   * echo -> listen: send message
   */
  static bool initState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    Echo* echo = (Echo*) fsm;
    trace.print(PSTR("init "));
    trace.print(echo->m_name);
    trace.println();
    fsm->set_state(listenState);
    return (true);
  }

  static bool listenState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    Echo* echo = (Echo*) fsm;
    trace.print(echo->m_name);
    trace.printf(PSTR("(count = %d)\n"), echo->m_count++);
    fsm->set_state(echoState);
    fsm->set_timer(512);
    return (true);
  }

  static bool echoState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    Echo* echo = (Echo*) fsm;
    echo->m_port->send(Event::USER_TYPE);
    fsm->set_state(listenState);
    return (true);
  };
};

// The three echo state machines
Echo ping;
Echo pong;
Echo pang;

void setup()
{
  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPingPong: started"));

  // Start the watchdog for timeout handling
  Watchdog::begin();

  // Bind the state machines to each other and give them names
  ping.bind(PSTR("ping"), &pong);
  pong.bind(PSTR("pong"), &pang);
  pang.bind(PSTR("pang"), &ping);

  // Start the state machines
  ping.begin();
  pong.begin();
  pang.begin();

  // Send an event to the first to kick them off
  ping.send(Event::USER_TYPE);
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
