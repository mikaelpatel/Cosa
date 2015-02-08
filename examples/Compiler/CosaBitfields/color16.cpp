/**
 * @file color16.cpp
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
 * Evaluate different methods of bit-field access using macro, enum,
 * shift and mask, and bit-fields struct definitions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "color16.h"

uint16_t
color16a(uint8_t red, uint8_t green, uint8_t blue)
{
  color16_t c;
  c.red = red >> 3;
  c.green = green >> 2;
  c.blue = blue >> 3;
  return (c.rgb);
}

uint16_t
color16b(uint8_t red, uint8_t green, uint8_t blue)
{
  return ((((red >> 3) & 0x1f) << 11)  |
	  (((green >> 2) & 0x3f) << 5) |
	  ((blue >> 3) & 0x1f));
}

