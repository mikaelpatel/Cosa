/**
 * @file CosaBlinkMinimal.ino
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
 * Cosa LED blink demonstration. The classical LED blink program
 * written in Cosa using the Arduino built-in LED and minimal number of
 * lines of code. Uses the OutputPin static member function toggle to
 * turn on and off the built-in LED. And the default delay function
 * which is a busy-wait loop.
 *
 * @section Circuit
 * Uses built-in LED (D13/Arduino).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"

void setup()
{
  OutputPin::mode(Board::LED);
}

void loop()
{
  OutputPin::toggle(Board::LED);
  delay(500);
}
