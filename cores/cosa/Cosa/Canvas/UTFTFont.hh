/**
 * @file Cosa/Canvas/UTFTFont.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CANVAS_UTFT_FONT_HH__
#define __COSA_CANVAS_UTFT_FONT_HH__

#include "Cosa/Types.h"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Font.hh"

/**
 * UTFT font library handler for Cosa Canvas. Needs a special 
 * handling of the font bitmap as the decoding is different.
 */
class UTFTFont : public Font {
public:
  /**
   * Construct UTFT font descriptor and bitmap.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] bitmap font storage.
   */
  UTFTFont(uint8_t width, uint8_t height, const uint8_t* bitmap) :
    Font(width, height, bitmap)
  {}
  
  /**
   * @override Font
   * Get bitmap for given character.
   * @param[in] c character.
   * @return bitmap pointer.
   */
  virtual const uint8_t* get_bitmap(char c)
  {
    return (m_bitmap + (c * HEIGHT) * (WIDTH / CHARBITS));
  }

  /**
   * @override Font
   * Draw character on given canvas.
   * @param[in] canvas
   * @param[in] c character.
   * @param[in] x position.
   * @param[in] y position.
   * @param[in] scale
   */
  virtual void draw(Canvas* canvas, char c, 
		    uint8_t x, uint8_t y, 
		    uint8_t scale);
};

#endif
