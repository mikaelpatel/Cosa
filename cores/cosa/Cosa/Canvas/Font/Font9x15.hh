/**
 * @file Font9x15.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, jediunix
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
 */

#ifndef COSA_FONT_FONT9X15_HH
#define COSA_FONT_FONT9X15_HH

#include "Cosa/Canvas/GLCDFont.hh"

/**
 * Font size 9x15.
 */
class Font9x15 : public GLCDFont {
public:
  /**
   * Construct font (9x15) singleton.
   */
  Font9x15() : GLCDFont(data[0], data[1], data[2], data[3], &data[4]) {}

private:
  static const uint8_t data[] PROGMEM;
};

extern Font9x15 font9x15;

#endif
