/**
 * @file Cosa/Font/FixedNum8x16.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * Bitmap font size 8x16, fixed with font with numbers only.
 *
 * @section Acknowledgement
 * Originates from the GLCD library and adapted for Cosa Canvas.
 * The GLCD library was created by Michael Margolis and improved 
 * by Bill Perry.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_FONT_FIXEDNUMS8X16_HH__
#define __COSA_FONT_FIXEDNUMS8X16_HH__

#include "Cosa/Font.hh"

class FixedNums8x16 : public Font {
private:
  static const uint8_t bitmap[] PROGMEM;

public:
  FixedNums8x16() : Font(8, 15, bitmap) {}

  /**
   * @overriden
   * Get bitmap for given character.
   * @param[in] c character.
   * @return bitmap pointer.
   */
  virtual const uint8_t* get_bitmap(char c)
  {
    c = c - '+';
    if (c > 16) c = 0;
    return (m_bitmap + (c * WIDTH)*((HEIGHT + 1)/CHARBITS));
  }

  /**
   * Draw character.
   * @param[in] canvas
   * @param[in] c
   * @param[in] x 
   * @param[in] y
   * @param[in] scale
   */
  virtual void draw(Canvas* canvas, char c, uint8_t x, uint8_t y, 
		    uint8_t scale)
  {
    canvas->draw_icon(x, y, get_bitmap(c), WIDTH, HEIGHT, scale);
  }
};

extern FixedNums8x16 fixednums8x16;

#endif
