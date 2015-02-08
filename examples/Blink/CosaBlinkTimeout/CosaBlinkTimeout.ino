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
#include "Cosa/Linkage.hh"
#include "Cosa/Watchdog.hh"

// LED output pin
class LED : public Link {
public:
  LED(Board::DigitalPin pin, uint8_t initial = 0) :
    Link(),
    m_pin(pin, initial)
  {
  }

  virtual void on_event(uint8_t type, uint16_t value)
  {
    UNUSED(type);
    UNUSED(value);
    m_pin.toggle();
  }

  void blink(uint16_t ms)
  {
    Watchdog::attach(this, ms);
  }

private:
  OutputPin m_pin;
};

// Use an RGB LED connected to pins(5,6,7)/ATtiny(1,2,3)
#if defined(BOARD_ATTINY)
LED redLedPin(Board::D1);
LED greenLedPin(Board::D2, 1);
LED blueLedPin(Board::D3);
#else
LED redLedPin(Board::D5);
LED greenLedPin(Board::D6, 1);
LED blueLedPin(Board::D7);
#endif

// And builtin LED
LED builtinPin(Board::LED);

void setup()
{
  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, Watchdog::push_timeout_events);

  // Set blink time period for the leds
  builtinPin.blink(1024);
  redLedPin.blink(512);
  greenLedPin.blink(1024);
  blueLedPin.blink(1024);
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
