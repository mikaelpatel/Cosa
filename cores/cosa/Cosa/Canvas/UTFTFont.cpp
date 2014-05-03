/**
 * @file Cosa/Canvas/UTFTFont.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Canvas/UTFTFont.hh"

void
UTFTFont::draw(Canvas* canvas, char c, 
	       uint8_t x, uint8_t y, 
	       uint8_t scale)
{
  const uint8_t* bp = get_bitmap(c);
  for (uint8_t i = 0; i < HEIGHT; i++) {
    for (uint8_t j = 0; j < WIDTH; j += 8) {
      uint8_t line = pgm_read_byte(bp++);
      for (uint8_t k = 0; k < 8; k++) {
	if (line & 0x80) {
	  if (scale == 1)
	    canvas->draw_pixel(x + j + k, y + i);
	  else {
	    canvas->fill_rect(x + (j + k)*scale, y + i*scale, scale, scale);
	  } 
	}
	line <<= 1;
      }
    }
  }
}
