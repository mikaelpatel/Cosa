/**
 * @file CosaBlinkEvent.ino
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
 * Cosa LED blink with watchdog timeout event wait for low power.
 * The classical LED blink program written in Cosa using the Arduino
 * built-in LED, Watchdog and Event queue to allow sleep mode during
 * LED pin toggle. Show the declarative and object-oriented style of
 * Cosa.  
 * 
 * @section Circuit
 * Uses built-in LED (D13/Arduino).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

// Use the build-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start the watchdog (0.5 second timeout, watchdog timeout events)
  Watchdog::begin(512, Watchdog::push_watchdog_event);
}

void loop()
{
  // Wait for timeout event 
  Event event;
  Event::queue.await(&event);
  
  // Toggle the led pin
  ledPin.toggle();
}
