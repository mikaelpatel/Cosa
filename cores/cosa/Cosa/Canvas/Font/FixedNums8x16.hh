/**
 * @file Cosa/Canvas/Font/FixedNums8x16.hh
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

#ifndef __COSA_CANVAS_FONT_FIXEDNUMS8X16_HH__
#define __COSA_CANVAS_FONT_FIXEDNUMS8X16_HH__

#include "Cosa/Canvas/GLCDFont.hh"

/**
 * GLCD bitmap font size 8x16, fixed width font with numbers only.
 *
 * @section Acknowledgements
 * Originates from the GLCD library and adapted for Cosa Canvas. The
 * GLCD library was created by Michael Margolis and improved by Bill
 * Perry. 
 */
class FixedNums8x16 : public GLCDFont {
public:
  /**
   * Construct fixed number font singleton.
   */
  FixedNums8x16() : GLCDFont(8, 15, bitmap) {}

  /**
   * @overriden Font
   * Get bitmap for given character. Must be ASCII '+'..'0'..'9'.
   * Returns pointer to bitmap in program memory.   
   * @param[in] c character.
   * @return bitmap pointer.
   */
  virtual const uint8_t* get_bitmap(char c)
  {
    c = c - '+';
    if (c > 16) c = 0;
    return (m_bitmap + (c * WIDTH)*((HEIGHT + 1)/CHARBITS));
  }

private:
  static const uint8_t bitmap[] PROGMEM;
};

extern FixedNums8x16 fixednums8x16;

#endif
