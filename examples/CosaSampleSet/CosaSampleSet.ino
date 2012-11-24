/**
 * @file CosaSampleSet.ino
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
 * Cosa demonstration of analog sample set.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Pins.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Trace.h"

// Analog input pins
AnalogPin levelPin(0);
AnalogPin basePin(1);
AnalogPin powerPin(2);
AnalogPin tempVCC(8);

// Analog pin vector for pin set. Note: use program memory
const PROGMEM AnalogPin* pins[] = {
  &levelPin,
  &basePin,
  &powerPin,
  &tempVCC
};

// Declare the pin set with vector, number of members and push event callback
AnalogPinSet pinSet(pins, membersof(pins), AnalogPinSet::push_event);

// Use the buildin led as a heartbeat
OutputPin ledPin(13, 0);

void setup()
{
  // Start the trace output stream
  trace.begin(9600, PSTR("CosaSampleSet: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(AnalogPin));
  TRACE(sizeof(AnalogPinSet));
  TRACE(sizeof(pins));
  TRACE(membersof(pins));

  // Start the watchdog ticks counter (shortest possible tick, 16 ms)
  Watchdog::begin(16);
}

void loop()
{
  // Start sampling analog pins and Wait for the next event
  ledPin.toggle();
  TRACE(pinSet.begin());
  Event event;
  Event::queue.await(&event);
  ledPin.toggle();

  // Print the values
  TRACE(levelPin.get_value());
  TRACE(basePin.get_value());
  TRACE(powerPin.get_value());
  TRACE(tempVCC.get_value());

  // Wait (approx. 0.5 s) before issuing the next sample
  Watchdog::delay(512);
}
