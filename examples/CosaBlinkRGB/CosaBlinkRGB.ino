/**
 * @file CosaBlinkRBG.ino
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
 * Cosa RGB LED blink demonstration using a Finite State Machine.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FSM.h"
#include "Cosa/Pins.h"
#include "Cosa/Watchdog.h"

// The state machine
FSM fsm;

void redState(FSM* fsm, uint8_t type, uint16_t value);
void greenState(FSM* fsm, uint8_t type, uint16_t value);
void blueState(FSM* fsm, uint8_t type, uint16_t value);

// Use an RGB LED connected to pins(5,6,7)
OutputPin redLedPin(5);
OutputPin greenLedPin(6);
OutputPin blueLedPin(7, 1);

// State functions; red, green and blue
void redState(FSM* fsm, uint8_t type, uint16_t value)
{
  blueLedPin.toggle();
  redLedPin.toggle();
  fsm->set_timer(128);
  fsm->set_state(greenState);
}

void greenState(FSM* fsm, uint8_t type, uint16_t value)
{
  redLedPin.toggle();
  greenLedPin.toggle();
  fsm->set_timer(256);
  fsm->set_state(blueState);
}

void blueState(FSM* fsm, uint8_t type, uint16_t value)
{
  greenLedPin.toggle();
  blueLedPin.toggle();
  fsm->set_timer(512);
  fsm->set_state(greenState);
}

void setup()
{
  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Start the state machine
  fsm.begin(redState);
}

void loop()
{
  // The basic event dispatcher
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}

