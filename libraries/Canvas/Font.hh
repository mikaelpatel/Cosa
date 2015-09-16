/**
 * @file Font.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_FONT_HH
#define COSA_FONT_HH

#include "Cosa/Types.h"
#include "Canvas.hh"

/*
 * Bitmap font library handler.
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
   * Construct font descriptor.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] first character available.
   * @param[in] last character available.
   * @param[in] bitmap font storage.
   * @param[in] compression_type.
   * @param[in] spacing recommended character spacing.
   * @param[in] line_spacing recommended line spacing.
   */
  Font(uint8_t width, uint8_t height,
       uint8_t first, uint8_t last,
       const uint8_t* bitmap,
       uint8_t compression_type = 0,
       uint8_t spacing = 1, uint8_t line_spacing = 1) :
    WIDTH(width),
    HEIGHT(height),
    SPACING(spacing),
    LINE_SPACING(line_spacing),
    FIRST(first),
    LAST(last),
    m_bitmap(bitmap),
    m_compression_type(compression_type)
  {
  }

  /**
   * @override{Font}
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
   * @override{Font}
   * Draw character on given canvas.
   * @param[in] canvas.
   * @param[in] c character.
   * @param[in] x position.
   * @param[in] y position.
   * @param[in] scale.
   */
  virtual void draw(Canvas* canvas, char c, uint16_t x, uint16_t y,
                    uint8_t scale);

  /**
   * Display a character.
   * Used in the form:
   *   Font::Glyph glyph(font, character);
   *   for (uint16_t i = 0; i < HEIGHT; i += 8) {
   *     for (uint16_t j = 0; j < WIDTH; j++) {
   *       uint8_t bits = glyph.next();
   *         // display bits
   *     }
   *   }
   */
  class Glyph {
  public:
    Glyph(Font* font, char c = ' ') :
      m_font(font),
      m_char(c)
    {
      begin(c);
    }

    /**
     * @override{Font::Glyph}
     * Begin character.
     * @param[in] c character.
     */
    void begin(char c);

    /**
     * @override{Font::Glyph}
     * Get next byte.
     * @return byte.
     */
    uint8_t next();

  protected:
    Font* m_font;
    char m_char;
    uint8_t m_offset;
    uint8_t m_flags;
    uint8_t* m_bitset;  // in progmem
    uint8_t* m_bitmap;  // in progmem
    uint8_t m_next;
  };

protected:
  /** Font bitmap. */
  const uint8_t* m_bitmap;

  /** Compression type. */
  const uint8_t m_compression_type;
};

#endif
