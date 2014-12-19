/**
 * @file Cosa/Canvas/UTFTFont.hh
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

#ifndef COSA_CANVAS_UTFTFONT_HH
#define COSA_CANVAS_UTFTFONT_HH

#include "Cosa/Types.h"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Font.hh"

/**
 * UTFT font library handler for Cosa Canvas.
 */
class UTFTFont : public Font {
public:
  /**
   * Construct font descriptor with data.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] first character available.
   * @param[in] last character available.
   * @param[in] data font storage.
   */
  UTFTFont(uint8_t width, uint8_t height,
           uint8_t first, uint8_t last,
           const uint8_t* data) :
    Font(width, height, first, last, data)
  {
  }

  /**
   * @deprecated As of 2014-12, use constructor with recommended character and line spacing.
   * Construct UTFT font descriptor and bitmap.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] data font storage.
   */
  UTFTFont(uint8_t width, uint8_t height, const uint8_t* data) __attribute__((deprecated)) :
    Font(width, height, 0, 127, data)
  {}
  
  /**
   * @deprecated As of 2014-12, use Glyph.
   * @override Font
   * Get bitmap for given character.
   * @param[in] c character.
   * @return bitmap pointer.
   */
  virtual const uint8_t* get_bitmap(char c)
    __attribute__((deprecated))
  {
    if (c > LAST)
      c = FIRST;
    return (m_data + ((c - FIRST) * HEIGHT * (WIDTH / CHARBITS)));
  }

protected:
  /**
   * @override Font
   * Render character into FontGlyph
   * @param[in] image FontGlyph image.
   * @param[in] size image size (must be WIDTH * ((HEIGHT+7)/8)).
   * @param[in] c character.
   */
  virtual void render(uint8_t* image, uint8_t size, char c);
};

#endif
