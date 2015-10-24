/**
 * @file CosaBlinkPeriodicBlock.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * The classic LED blink example using a periodic block.
 *
 * @section Circuit
 * Uses built-in LED (D13/Arduino).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/OutputPin.hh"

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start the real-time timer for periodic blocks
  RTT::begin();
}

void loop()
{
  // Blink the built-in LED with approx. 1 Hz
  periodic(blink, 500) {
    ledPin.toggle();
  }
}
