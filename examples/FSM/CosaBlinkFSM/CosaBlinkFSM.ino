/**
 * @file CosaBlinkFSM.ino
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

#include "Cosa/FSM.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"

// The state machine: Blink RGB LED with six color states
class BlinkRGB : public FSM {

public:
  // The output pins
  OutputPin redLedPin;
  OutputPin greenLedPin;
  OutputPin blueLedPin;

  // Construct the state machine for the RGB led sequencing
  BlinkRGB(uint16_t period = 512,
	   uint8_t redLedPinNr = 5, 
	   uint8_t greenLedPinNr = 6, 
	   uint8_t blueLedPinNr= 7) :
    FSM(redState, period),
    redLedPin(redLedPinNr, 1),
    greenLedPin(greenLedPinNr),
    blueLedPin(blueLedPinNr, 1)
  {}

  // State functions; red => yellow => green => cyan => blue => meganta
  // Receive a timeout events which are ignored. Turns on and off the
  // leds with toggle and steps to the next state. 
  static bool redState(FSM* fsm, uint8_t type)
  {
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->blueLedPin.toggle();
    fsm->set_state(yellowState);
    return (1);
  }

  static bool yellowState(FSM* fsm, uint8_t type)
  {
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->greenLedPin.toggle();
    fsm->set_state(greenState);
    return (1);
  }
  
  static bool greenState(FSM* fsm, uint8_t type)
  {
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->redLedPin.toggle();
    fsm->set_state(cyanState);
    return (1);
  }
  
  static bool cyanState(FSM* fsm, uint8_t type)
  {
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->blueLedPin.toggle();
    fsm->set_state(blueState);
    return (1);
  }

  static bool blueState(FSM* fsm, uint8_t type)
  {
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->greenLedPin.toggle();
    fsm->set_state(magentaState);
    return (1);
  }

  static bool magentaState(FSM* fsm, uint8_t type)
  {
    BlinkRGB* rgb = (BlinkRGB*) fsm;
    rgb->redLedPin.toggle();
    fsm->set_state(redState);
    return (1);
  }
};

// The state machines for two RGB leds
BlinkRGB led1(512, 5, 6, 7);
BlinkRGB led2(256, 8, 9, 10);

void setup()
{
  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Start the state machines
  led1.begin();
  led2.begin();
}

void loop()
{
  // The basic event dispatcher
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}

