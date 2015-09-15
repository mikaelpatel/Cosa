/**
 * @file HD44780_IO.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "HD44780.hh"

void
HD44780::IO::write8b(uint8_t data)
{
  write4b(data >> 4);
  write4b(data);
}

void
HD44780::IO::write8n(const void* buf, size_t size)
{
  const uint8_t* bp = (const uint8_t*) buf;
  while (size--) write8b(*bp++);
}

