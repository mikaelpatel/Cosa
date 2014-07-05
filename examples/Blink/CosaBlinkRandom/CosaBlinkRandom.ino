/**
 * @file CosaBlinkRandom.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/OutputPin.hh"

OutputPin ledPin(Board::LED);

int32_t 
random(int32_t range)
{
  if (range == 0) return 0;
  return (random() % range);
}

int32_t
random(int32_t low, int32_t high)
{
  if (low >= high) return (low);
  return (random(high - low) + low);
}

void loop()
{
  ledPin.high();
  delay(random(100, 500));
  ledPin.low();
  delay(random(100, 500));
}
