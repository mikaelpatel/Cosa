/**
 * @file Cosa/Canvas/Font.hh
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

#ifndef COSA_CANVAS_FONT_HH
#define COSA_CANVAS_FONT_HH

#include "Cosa/Types.h"
#include "Cosa/Canvas.hh"

/*
 * Bitmap font library handler for Cosa Canvas.
 */
class Font {
public:
  /**
   * Font size; width/height and character/line spacing.
   */
  const uint8_t WIDTH;
  const uint8_t HEIGHT;
  const uint8_t SPACING;
  uint8_t LINE_SPACING;

  /**
   * Font range.
   */
  const uint8_t FIRST;
  const uint8_t LAST;

  /**
   * Construct font descriptor and bitmap.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] first character available.
   * @param[in] last character available.
   * @param[in] bitmap font storage.
   * @param[in] spacing recommended character spacing.
   * @param[in] line_spacing recommended line spacing.
   */
  Font(uint8_t width, uint8_t height,
       uint8_t first, uint8_t last,
       const uint8_t* bitmap,
       uint8_t spacing = 1, uint8_t line_spacing = 1) :
    WIDTH(width), 
    HEIGHT(height),
    SPACING(spacing),
    LINE_SPACING(line_spacing),
    FIRST(first),
    LAST(last),
    m_bitmap(bitmap)
  {
  }

  /**
   * @deprecated As of 2014-12, use constructor with first/last indicators.
   * Construct font descriptor and bitmap.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] bitmap font storage.
   */
  Font(uint8_t width, uint8_t height, const uint8_t* bitmap) __attribute__((deprecated)) :
    WIDTH(width), 
    HEIGHT(height),
    SPACING(1),
    LINE_SPACING(0),
    FIRST(0),
    LAST(127),
    m_bitmap(bitmap)
  {
  }
  
  /**
   * @override Font
   * Determine if character is available in font.
   * @param[in] c character.
   * @return true if available.
   */
  bool available(char c)
    __attribute__((always_inline))
  {
    return (c >= FIRST && c <= LAST);
  }

  /**
   * @override Font
   * Get bitmap for given character.
   * @param[in] c character.
   * @return bitmap pointer.
   */
  virtual const uint8_t* get_bitmap(char c)
  {
    return (m_bitmap + ((c - FIRST) * WIDTH * ((HEIGHT + (CHARBITS-1)) / CHARBITS)));
  }

  /**
   * @deprecated As of 2014-12, because SPACING is included, use WIDTH+SPACING.
   * @override Font
   * Get width for given character.
   * @param[in] c character.
   * @return width.
   */
  virtual uint8_t get_width(char c)
    __attribute__((deprecated))
  {
    UNUSED(c);
    return (WIDTH + SPACING);
  }
  
  /**
   * @deprecated As of 2014-12, because SPACING is included, use (WIDTH+SPACING)*strlen(s).
   * @override Font
   * Get width for given string.
   * @param[in] s string.
   * @return width.
   */
  virtual uint8_t get_width(char* s)
    __attribute__((deprecated))
  {
    return ((WIDTH + SPACING) * strlen(s));
  }
  
  /**
   * @deprecated As of 2014-12, because SPACING is included, use (WIDTH+SPACING)*strlen_P(s).
   * @override Font
   * Get width for given string in program memory.
   * @param[in] s string in program memory.
   * @return width.
   */
  virtual uint8_t get_width_P(const char* s)
    __attribute__((deprecated))
  {
    return ((WIDTH + SPACING) * strlen_P(s));
  }
  
  /**
   * @deprecated As of 2014-12, because of name style inconsistency, use HEIGHT.
   * @override Font
   * Get width for given character.
   * @param[in] c character.
   * @return height.
   */
  virtual uint8_t get_height(char c)
    __attribute__((deprecated))
  {
    UNUSED(c);
    return (HEIGHT);
  }

  /**
   * @override Font
   * Draw character on given canvas.
   * @param[in] canvas.
   * @param[in] c character.
   * @param[in] x position.
   * @param[in] y position.
   * @param[in] scale.
   */
  virtual void draw(Canvas* canvas, char c, uint16_t x, uint16_t y, 
		    uint8_t scale)
  {
    canvas->draw_bitmap(x, y, get_bitmap(c), WIDTH, HEIGHT, scale);
  }

protected:
  /** Font bitmap. */
  const uint8_t* m_bitmap;
};

#endif
