/**
 * @file Cosa/Canvas/Font.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/FontGlyph.hh"

void
Font::draw(Canvas* canvas, char c, uint8_t x, uint8_t y, 
           uint8_t scale)
{
  FontGlyph glyph(this, c);
  canvas->draw_glyph(x, y, (Glyph*)&glyph, scale);
}

void
Font::render(uint8_t* image, uint8_t size, char c)
{
  if (!image || size != (WIDTH * GLYPH_BITS_TO_BYTES(HEIGHT)))
    return;

  if (!present(c))
    {
      memset(image, 0, size);
      return;
    }

  const uint8_t* bp = m_data + (c - FIRST) * WIDTH * GLYPH_BITS_TO_BYTES(HEIGHT);

  for (uint8_t x = 0; x < WIDTH; x++)
    for (uint8_t yb = 0; yb < GLYPH_BITS_TO_BYTES(HEIGHT); yb++)
      image[(yb * WIDTH) + x] = pgm_read_byte(bp++);
}
