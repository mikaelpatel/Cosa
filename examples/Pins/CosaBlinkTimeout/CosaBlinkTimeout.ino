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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa RGB LED blink with timeout events.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"

// Use an RGB LED connected to pins(5,6,7)
OutputPin redLedPin(5);
OutputPin greenLedPin(6, 1);
OutputPin blueLedPin(7);

// Event handler for toggling the output pin
void blink(Thing* it, uint8_t type, uint16_t value)
{
  OutputPin* pin = (OutputPin*) it;
  pin->toggle();
}

void setup()
{
  // Set the blink function as the led event handler
  redLedPin.set_event_handler(blink);
  greenLedPin.set_event_handler(blink);
  blueLedPin.set_event_handler(blink);

  // Add the led pins to watchdog timeout periods
  Watchdog::attach(&redLedPin, 512);
  Watchdog::attach(&greenLedPin, 1024);
  Watchdog::attach(&blueLedPin, 1024);

  // Start the watchdog (16 ms timeout, push timeout events)
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
}

void loop()
{
  // The basic event dispatcher
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
