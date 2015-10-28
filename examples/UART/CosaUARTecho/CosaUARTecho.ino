/**
 * @file CosaUARTecho.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Test Cosa UART ability to read and write at high speed.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/UART.hh"

void setup()
{
  uart.begin(230400);
}

void loop()
{
  while (uart.available())
    uart.putchar(uart.getchar());
}
