/**
 * @file CosaBlinkRandom.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Cosa LED blink demonstration with a twist. The classical Arduino
 * LED blink program written in Cosa using the Arduino built-in LED
 * with a random delay. No setup is necessary as mode is handled by
 * the OutputPin class. The delay is busy-wait (not low power mode).
 *
 * @section Circuit
 * Uses built-in LED (D13/Arduino).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Math.hh"
#include "Cosa/OutputPin.hh"

OutputPin ledPin(Board::LED);

void loop()
{
  ledPin.on();
  delay(rand(100, 500));
  ledPin.off();
  delay(rand(100, 500));
}
