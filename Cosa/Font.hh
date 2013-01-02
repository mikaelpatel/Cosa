/**
 * @file Cosa/Font.hh
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
 * Bitmap font library.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_FONT_HH__
#define __COSA_FONT_HH__

#include "Cosa/Types.h"

class Font {

private:
  /**
   * Font bitmap.
   */
  const uint8_t* m_bitmap;

public:
  /**
   * Font size; width/height 
   */
  const uint8_t WIDTH;
  const uint8_t HEIGHT;

  /**
   * Construct font descriptor and bitmap.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] bitmap font storage.
   */
  Font(uint8_t width, uint8_t height, const uint8_t* bitmap) :
    m_bitmap(bitmap),
    WIDTH(width), 
    HEIGHT(height)
  {}
  
  /**
   * Get bitmap for given character.
   * @param[in] c character.
   * @return bitmap pointer.
   */
  const uint8_t* get_bitmap(char c)
  {
    return (m_bitmap + (c*WIDTH));
  }
};

#endif
