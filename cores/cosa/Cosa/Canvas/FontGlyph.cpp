/**
 * @file Cosa/Canvas/FontGlyph.cpp
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


FontGlyph::FontGlyph(Font* font, char c) :
  Glyph(font->WIDTH, font->HEIGHT),
  m_font(font),
  m_character(c)
{
  character(c);
}

void
FontGlyph::character(char c)
{
  end();

  m_character = c;

  m_font->render(m_image, WIDTH * GLYPH_BITS_TO_BYTES(HEIGHT), c);
}

uint8_t
FontGlyph::spacing()
{
  return (m_font->SPACING);
}

uint8_t
FontGlyph::line_spacing()
{
  return (m_font->LINE_SPACING);
}
