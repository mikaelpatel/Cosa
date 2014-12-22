/**
 * @file Cosa/Canvas/IconGlyph.cpp
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

#include "Cosa/Canvas/IconGlyph.hh"


IconGlyph::IconGlyph(uint8_t width, uint8_t height, const uint8_t* data) :
  Glyph(width, height),
  m_data(data)
{
  /* Render icon */

  if (!m_image)
    return;

  const uint8_t* bp = m_data;

  // Renders like GLCDFont
  for (uint8_t yb = 0; yb < GLYPH_BITS_TO_BYTES(HEIGHT); yb++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      uint8_t element = pgm_read_byte(bp++);
      m_image[(yb * WIDTH) + x] = element;
    }
  }
}
