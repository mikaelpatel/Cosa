/**
 * @file CosaButton.ino
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
 * Cosa demonstration of button abstraction.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Button.h"
#include "Cosa/Pins.h"
#include "Cosa/Event.h"
#include "Cosa/Memory.h"

// Input pin and button
InputPin onoffPin(7, InputPin::PULLUP_MODE);
Button onoffButton(Button::ON_FALLING_MODE);

// Use the built-in led
OutputPin ledPin(13);

void on_change(Thing* it, uint8_t type, uint16_t value)
{
  static uint8_t count = 1;
  ledPin.toggle();
  TRACE(count++);
  Watchdog::delay(16);
  ledPin.toggle();
}

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaButton: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(InputPin));
  TRACE(sizeof(Button));

  // Set the event callback and attach the input pin
  onoffPin.set_event_handler(on_change);
  onoffButton.attach(&onoffPin);

  // Start the watchdog ticks and push time events
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
}

void loop()
{
  // The basic event top loop
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
