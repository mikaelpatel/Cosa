/**
 * @file CosaBlink.ino
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
 * Cosa LED blink demonstration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start the watchdog with the default timeout period (16 ms)
  Watchdog::begin();
}

void loop()
{
  // Turn on the led for 16 ms
  ledPin.on();
  Watchdog::delay(16);

  // Turn off the led and wait 512 ms
  ledPin.off();
  Watchdog::delay(512);
}
