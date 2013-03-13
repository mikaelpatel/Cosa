/**
 * @file CosaBlinkTimeout.ino
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
 * @section Description
 * Cosa RGB LED blink with timeout events.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Watchdog.hh"

// LED output pin
class LED : public Link {
private:
  OutputPin m_pin;
public:
  LED(Board::DigitalPin pin, uint8_t initial = 0) : 
    Link(),
    m_pin(pin, initial)
  {
  }

  virtual void on_event(uint8_t type, uint16_t value)
  {
    m_pin.toggle();
  }

  void blink(uint16_t ms) 
  {
    Watchdog::attach(this, ms);
  }
};

// Use an RGB LED connected to pins(5,6,7)
LED redLedPin(Board::D5);
LED greenLedPin(Board::D6, 1);
LED blueLedPin(Board::D7);

// And builtin LED
LED builtinPin(Board::LED);

void setup()
{
  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Set blink time period for the leds
  builtinPin.blink(1024);
  redLedPin.blink(512);
  greenLedPin.blink(1024);
  blueLedPin.blink(1024);
}

void loop()
{
  // The basic event dispatcher
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
