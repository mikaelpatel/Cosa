/**
 * @file CosaBlinkPeriodic.ino
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
 * Cosa RGB LED blink with periodic function.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Periodic.hh"
#include "Cosa/Pins.hh"

// Blinking LED output pin
class LED : public Periodic {
private:
  OutputPin m_pin;
public:
  LED(Board::DigitalPin pin, uint16_t ms, uint8_t initial = 0) : 
    Periodic(ms), m_pin(pin, initial) {}
  virtual void run() { m_pin.toggle(); }
};

// Use an RGB LED connected to pins(5,6,7)
LED redLedPin(Board::D5, 512);
LED greenLedPin(Board::D6, 1024, 1);
LED blueLedPin(Board::D7, 1024);

// Start the watchdog (16 ms timeout, push timeout events)
void setup()
{
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
}

// The basic event dispatcher
void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
