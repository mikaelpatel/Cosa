/**
 * @file CosaBlinkFSM.ino
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
 * Cosa RGB LED blink demonstration using a Finite State Machine; The
 * classical LEB blink example program with two RGB LEDs controlled by
 * an FSM for each color state on each LED. The two RGB LEDs will also
 * blink with two different periods.
 *
 * @section Circuit
 * Connect two RGB LEDs to pins (D5, D6, D7) and (D8, D9, D10).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FSM.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

/**
 * The state machine: Blink RGB LED with six color states
 * @dot
 * digraph CosaBlinkFSM {
 *   RED -> YELLOW [label="timeout"];
 *   YELLOW -> GREEN [label="timeout"];
 *   GREEN -> CYAN [label="timeout"];
 *   CYAN -> BLUE [label="timeout"];
 *   BLUE -> MAGENTA [label="timeout"];
 *   MAGENTA -> RED [label="timeout"];
 * }
 * @enddot
 */
class BlinkRGB : public FSM {
public:
  // Construct the state machine for the RGB led sequencing
  BlinkRGB(Job::Scheduler* scheduler,
	   uint16_t period = 512,
	   Board::DigitalPin redLedPinNr = Board::D5,
	   Board::DigitalPin greenLedPinNr = Board::D6,
	   Board::DigitalPin blueLedPinNr = Board::D7) :
    FSM(redState, scheduler, period),
    redLedPin(redLedPinNr, 1),
    greenLedPin(greenLedPinNr),
    blueLedPin(blueLedPinNr, 1)
  {}

  str_P state_str_P()
  {
    StateHandler fn = state();
    if (fn == redState)
      return PSTR("red");
    if (fn == yellowState)
      return PSTR("yellow");
    if (fn == greenState)
      return PSTR("green");
    if (fn == cyanState)
      return PSTR("cyan");
    if (fn == blueState)
      return PSTR("blue");
    if (fn == magentaState)
      return PSTR("magenta");
    return PSTR("unknown");
  }

  // State functions; red => yellow => green => cyan => blue => meganta
  // Receive a timeout events which are ignored. Turns on and off the
  // leds with toggle and steps to the next state.
  static bool redState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->blueLedPin.toggle();
    fsm->state(yellowState);
    return (1);
  }

  static bool yellowState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->greenLedPin.toggle();
    fsm->state(greenState);
    return (1);
  }

  static bool greenState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->redLedPin.toggle();
    fsm->state(cyanState);
    return (1);
  }

  static bool cyanState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->blueLedPin.toggle();
    fsm->state(blueState);
    return (1);
  }

  static bool blueState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->greenLedPin.toggle();
    fsm->state(magentaState);
    return (1);
  }

  static bool magentaState(FSM* fsm, uint8_t type)
  {
    UNUSED(type);
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->redLedPin.toggle();
    fsm->state(redState);
    return (1);
  }

private:
  OutputPin redLedPin;
  OutputPin greenLedPin;
  OutputPin blueLedPin;
};

// Use the Watchdog job scheduler
Watchdog::Scheduler scheduler;

// The state machines for two RGB leds
BlinkRGB led1(&scheduler, 1024, Board::D5, Board::D6, Board::D7);
BlinkRGB led2(&scheduler, 512, Board::D8, Board::D9, Board::D10);

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaFSM: started"));
  trace.flush();

  // Start the state machines
  led1.begin();
  led2.begin();

  // Start the watchdog and set the scheduler
  Watchdog::begin();
}

void loop()
{
  // Wait for events
  Event event;
  Event::queue.await(&event);

  // Print the event target and current state
  BlinkRGB* led = (BlinkRGB*) event.target();
  trace << Watchdog::millis()
	<< PSTR(":FSM@") << led
	<< PSTR(",state=(") << led->state_str_P();

  // Dispatch the event and state transition
  event.dispatch();

  // Print the new state
  trace << PSTR(")=>(") << led->state_str_P() << PSTR(")") << endl;
}

