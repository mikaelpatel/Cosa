/**
 * @file CosaBlinkPeriodic.ino
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
 * The classic LED blink example using a periodic function handler.
 *
 * @section Circuit
 * Connect an RGB LED to pins (D5,D6,D7/Arduino, D1,D2,D3/Tiny).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/OutputPin.hh"

// Blinking LED output pin
class LED : public Periodic, private OutputPin {
public:
  LED(Board::DigitalPin pin,
      Job::Scheduler* scheduler, uint16_t ms,
      uint8_t initial = 0) :
    Periodic(scheduler, ms),
    OutputPin(pin, initial)
  {}

  virtual void run()
  {
    toggle();
  }
};

// Use watchdog job scheduler for periodic functions
Watchdog::Scheduler scheduler;

// RGB LED connected to Arduino pins
#if defined(BOARD_ATTINY)
LED redLedPin(Board::D1, &scheduler, 512);
LED greenLedPin(Board::D2, &scheduler, 1024, 1);
LED blueLedPin(Board::D3, &scheduler, 1024);
#else
LED redLedPin(Board::D5, &scheduler, 512);
LED greenLedPin(Board::D6, &scheduler, 1024, 1);
LED blueLedPin(Board::D7, &scheduler, 1024);
#endif

void setup()
{
  // Start the periodic functions
  redLedPin.start();
  greenLedPin.start();
  blueLedPin.start();

  // Start the watchdog ticks
  Watchdog::begin();
}

void loop()
{
  Event::service();
}
