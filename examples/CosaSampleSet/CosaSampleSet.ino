/**
 * @file CosaSampleSet.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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

AnalogPinSet pinSet(pins, membersof(pins), AnalogPinSet::push_event);

void setup()
{
  // Start the serial interface for trace output
  Serial.begin(9600);

  // Check amount of free memory
  Serial_trace(free_memory());

  // Check size of instances
  Serial_trace(sizeof(AnalogPin));
  Serial_trace(sizeof(AnalogPinSet));
  Serial_trace(sizeof(pins));
  Serial_trace(membersof(pins));

  // Start the watchdog ticks counter (64 ms pulse)
  Watchdog::begin(16);

  // Give to serial interface some time
  Watchdog::delay(128);
}

void loop()
{
  // Start sampling analog pins
  Serial_trace(pinSet.begin());

  // Wait for the next event. Allow a low power sleep
  Event event;
  Event::queue.await(&event);

  // Print the values
  Serial_trace(levelPin.get_value());
  Serial_trace(basePin.get_value());
  Serial_trace(powerPin.get_value());
  Serial_trace(tempVCC.get_value());

  // Wait (approx. 1 s) before issuing the next sample
  Watchdog::delay(1024);
}
