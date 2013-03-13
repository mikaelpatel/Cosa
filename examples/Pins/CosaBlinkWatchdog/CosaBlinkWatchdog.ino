/**
 * @file CosaBlinkWatchdog.ino
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
 * Cosa LED blink with watchdog timeout for low power.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"

// Use the builtin led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start the watchdog (0.5 second timeout)
  Watchdog::begin(512);
}

void loop()
{
  // Wait for watchdog tick
  Watchdog::await();
  
  // Toggle the led pin
  ledPin.toggle();
}
