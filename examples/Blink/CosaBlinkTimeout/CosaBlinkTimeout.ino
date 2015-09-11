/**
 * @file CosaBlinkTimeout.ino
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
 * Cosa RGB LED blink with timeout events.
 *
 * @section Circuit
 * Connect an RGB LED to pins (D5,D6,D7/Arduino, D1,D2,D3/Tiny).
 * Uses the built-in LED (D13/Arduino).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Event.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Watchdog.hh"

// Watchdog job scheduler
Watchdog::Scheduler scheduler;

// LED output pin
class LED : public Periodic, private OutputPin {
public:
  LED(Board::DigitalPin pin, uint32_t ms, uint8_t initial = 0) :
    Periodic(&scheduler, ms), OutputPin(pin, initial) {}
  virtual void run() { toggle(); }
};

// Use an RGB LED connected to pins(5,6,7)/ATtiny(1,2,3)
#if defined(BOARD_ATTINY)
LED redLedPin(Board::D1, 512);
LED greenLedPin(Board::D2, 1024, 1);
LED blueLedPin(Board::D3, 1024);
#else
LED redLedPin(Board::D5, 512);
LED greenLedPin(Board::D6, 1024, 1);
LED blueLedPin(Board::D7, 1024);
#endif

// And builtin LED
LED builtinPin(Board::LED, 1024);

void setup()
{
  // Start the watchdog timer and job scheduler
  Watchdog::begin();

  // Start the periodic functions.
  builtinPin.start();
  redLedPin.start();
  greenLedPin.start();
  blueLedPin.start();
}

void loop()
{
  // Dispatch events
  Event::service();
}
