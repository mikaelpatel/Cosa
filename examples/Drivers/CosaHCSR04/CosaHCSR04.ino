/**
 * @file CosaHCSR04.ino
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
 * Cosa demonstration of the Ultrasonic range module 
 * HC-SR04/US-020 device driver. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/HCSR04.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Connect to HCSR04 to pin 2 and 3. Use builtin led for heartbeat
HCSR04 ping(2, 3);
OutputPin ledPin(13);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaHCSR04: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(Thing));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(HCSR04));

  // Start the watchdog ticks and push time events
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Attach the range module to read distance every second
  Watchdog::attach(&ping, 1024);
}

void loop()
{
  // Wait and dispatch event for reading of distance
  Event event;
  Event::queue.await(&event);
  event.dispatch();

  // Print the distance from the latest reading
  ledPin.toggle();
  TRACE(ping.get_distance());
  ledPin.toggle();
}
