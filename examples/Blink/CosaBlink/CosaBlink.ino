/**
 * @file CosaBlink.ino
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
 * Cosa LED blink demonstration. The classical LED blink program
 * written in Cosa using the Arduino built-in LED with a very short
 * blink period(16:512 ms) and the Watchdog for low power mode during
 * delay.  
 *
 * @section Circuit
 * Uses built-in LED (D13/Arduino).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start the watchdog with the default timeout period (16 ms)
  // Will install low-power version of the delay() function
  Watchdog::begin();
}

void loop()
{
  // Turn on the led for 16 ms
  ledPin.on();
  delay(16);

  // Turn off the led and wait 512 ms
  ledPin.off();
  delay(512);
}
