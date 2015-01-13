/**
 * @file Cosa/Canvas/Font.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "Cosa/Canvas/Font.hh"

void
Font::draw(Canvas* canvas, char c, uint16_t x, uint16_t y,
           uint8_t scale)
{
  display_iterator_t iterator = display_begin(c);

  if (scale == 1) {
    for (uint16_t i = 0; i < HEIGHT; i += 8) {
      for (uint16_t j = 0; j < WIDTH; j++) {
        uint8_t bits = display_next(&iterator);
        if (bits == 0xff) {
          canvas->draw_vertical_line(x + j, y + i, CHARBITS);
        }
        else {
          for (uint8_t k = 0; k < 8; k++) {
            if (bits == 0) break;
            if (bits & 1) canvas->draw_pixel(x + j, y + k + i);
            bits >>= 1;
          }
        }
      }
    }
  }
  else {
    for (uint16_t i = 0; i < HEIGHT; i += 8) {
      for (uint16_t j = 0; j < WIDTH; j++) {
        uint8_t bits = display_next(&iterator);
        for (uint8_t k = 0; k < 8; k++) {
          if (bits == 0) break;
          if (bits & 1) canvas->fill_rect(x + j*scale, y + (k+i)*scale, scale, scale);
          bits >>= 1;
        }
      }
    }
  }
}

Font::display_iterator_t
Font::display_begin(char c)
{
  uint16_t chr = c;

  if (chr < FIRST || chr > LAST)
    return (NULL);

  return ((uint8_t*)m_bitmap + ((chr - FIRST) * WIDTH * ((HEIGHT + (CHARBITS-1)) / CHARBITS)));
}

uint8_t
Font::display_next(display_iterator_t* iterator)
{
  if (!*iterator)
    return (0x55);  // character not available in font shows streaks

  return (pgm_read_byte((*iterator)++));
}
