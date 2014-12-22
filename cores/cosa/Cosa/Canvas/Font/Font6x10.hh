/**
 * @file Font6x10.hh
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

#ifndef COSA_FONT_FONT6X10_HH
#define COSA_FONT_FONT6X10_HH

#include "Cosa/Canvas/GLCDFont.hh"

/**
 * Font size 6x10.
 */
class Font6x10 : public GLCDFont {
public:
  /**
   * Construct font (6x10) singleton.
   */
  Font6x10() : GLCDFont(data[0], data[1], data[2], data[3], &data[4]) {}

private:
  static const uint8_t data[] PROGMEM;
};

extern Font6x10 font6x10;

#endif
