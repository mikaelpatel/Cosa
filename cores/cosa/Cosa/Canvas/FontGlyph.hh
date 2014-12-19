/**
 * @file Cosa/Canvas/FontGlyph.hh
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

#ifndef COSA_CANVAS_FONTGLYPH_HH
#define COSA_CANVAS_FONTGLYPH_HH

#include "Cosa/Types.h"
#include "Cosa/Canvas/Glyph.hh"

class Font;

class FontGlyph : public Glyph {
public:
  /**
   * Construct font glyph.
   * @param[in] font font.
   * @param[in] c character.
   */
  FontGlyph(Font* font, char c = ' ');

  /**
   * Change to different character.
   * @param[in] c character.
   */
  void character(char c);

  /**
   * Get character spacing.
   * @return character spacing.
   */
  uint8_t spacing();
  
  /**
   * Get line spacing.
   * @return line spacing.
   */
  uint8_t line_spacing();
    
protected:
  /** Font for this glyph.*/
  Font* m_font;
  
  /** Character for this glyph.*/
  uint8_t m_character;
};

#endif
