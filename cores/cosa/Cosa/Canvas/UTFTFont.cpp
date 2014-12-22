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

#include "Cosa/Canvas/Glyph.hh"
#include "Cosa/Canvas/UTFTFont.hh"

void
UTFTFont::render(uint8_t* image, uint8_t size, char c)
{
  if (!image || size != (WIDTH * GLYPH_BITS_TO_BYTES(HEIGHT)))
    return;

  memset(image, 0, size);

  if (!present(c))
    return;

  const uint8_t* bp = m_data + ((c - FIRST) * GLYPH_BITS_TO_BYTES(WIDTH) * HEIGHT);

  
  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x += 8) {
      uint8_t line = pgm_read_byte(bp++);
      for (uint8_t b = 0; b < 8; b++) {
        uint16_t offset = ((GLYPH_BITS_TO_BYTES(y+1)-1) * WIDTH) + ((GLYPH_BITS_TO_BYTES(x+1)-1)*8) + b;
        uint16_t shift = y - ((GLYPH_BITS_TO_BYTES(y+1)-1)*8);
        if (line & 0x80)
          image[offset] |= 1 << shift;
        line <<= 1;
      }
    }
  }
}
