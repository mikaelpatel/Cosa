/**
 * @file Cosa/Canvas/IconGlyph.hh
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

#ifndef COSA_CANVAS_ICONGLYPH_HH
#define COSA_CANVAS_ICONGLYPH_HH

#include "Cosa/Types.h"
#include "Cosa/Canvas/Glyph.hh"

class IconGlyph : public Glyph {
public:
  /**
   * Construct glyph.
   * @param[in] font font.
   * @param[in] c character.
   * @param[in] data.
   */
  IconGlyph(uint8_t width, uint8_t height, const uint8_t* data);

protected:
  /** Data for this glyph.*/
  const uint8_t* m_data;
};

#endif
