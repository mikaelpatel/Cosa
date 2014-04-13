/**
 * @file Cosa/Canvas/Font/Segment32x50.hh
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CANVAS_FONT_SEGMENT32X50_HH__
#define __COSA_CANVAS_FONT_SEGMENT32X50_HH__

#include "Cosa/Canvas/UTFTFont.hh"

/**
 * UTFT bitmap font size 32x50, segments, fixed with font with 
 * numbers only.
 *
 * @section Acknowledgements
 * Font data originates from the UTFT library was created by 
 * Henning Karlsen.
 */
class Segment32x50 : public UTFTFont {
public:
  /** 
   * Construct large segment font (32x50) singleton.
   */
  Segment32x50() : 
    UTFTFont(32, 50, bitmap) 
  {
  }

  /**
   * @override Font
   * Get bitmap for given character ('0'..'9', ':'). Returns pointer
   * to bitmap in program memory.
   * @param[in] c character.
   * @return bitmap pointer.
   */
  virtual const uint8_t* get_bitmap(char c)
  {
    c = c - '0';
    if (c > 9) c = 10;
    return (m_bitmap + (c * HEIGHT) * (WIDTH / CHARBITS));
  }

private:
  static const uint8_t bitmap[] PROGMEM;
};

extern Segment32x50 segment32x50;

#endif
