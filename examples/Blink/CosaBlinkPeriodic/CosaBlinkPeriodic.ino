/**
 * @file CosaBlinkPeriodic.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
class LED : public Periodic {
public:
  LED(Board::DigitalPin pin, uint16_t ms, uint8_t initial = 0) : 
    Periodic(ms), m_pin(pin, initial) {}
  virtual void run() { m_pin.toggle(); }
private:
  OutputPin m_pin;
};

// RGB LED connected to Arduino pins
#if defined(__ARDUINO_TINY__)
LED redLedPin(Board::D1, 512);
LED greenLedPin(Board::D2, 1024, 1);
LED blueLedPin(Board::D3, 1024);
#else
LED redLedPin(Board::D5, 512);
LED greenLedPin(Board::D6, 1024, 1);
LED blueLedPin(Board::D7, 1024);
#endif

void setup()
{
  // Start the watchdog ticks and push time events
  Watchdog::begin(16, Watchdog::push_timeout_events);

  // Start the periodic functions
  redLedPin.begin();
  greenLedPin.begin();
  blueLedPin.begin();
}

void loop()
{
  // Wait for events (low power mode) and dispatch
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
