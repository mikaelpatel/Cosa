/**
 * @file CosaBlinkInterrupt.ino
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
 * Cosa LED blink with watchdog timeout interrupt callback.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.h"
#include "Cosa/Watchdog.h"

// Use the Arduino builtin LED
OutputPin ledPin(13, 0);

// Blink on timeout interrupt callback
void blink(void* env)
{
  ledPin.toggle();
}

// Start watchdog with approx. 1 second timeout and blink interrupt call
void setup()
{
  Watchdog::begin(1024, blink);
}

// Go into sleep mode while awaiting the next timeout
void loop()
{
  Watchdog::await();
}
