/**
 * @file Cosa/Canvas/Font/FixedNums8x16.hh
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

#ifndef COSA_CANVAS_FONT_FIXEDNUMS8X16_HH
#define COSA_CANVAS_FONT_FIXEDNUMS8X16_HH

#include "Cosa/Canvas/GLCDFont.hh"

/**
 * GLCD bitmap font size 8x16, fixed width font with numbers only.
 *
 * @section Acknowledgements
 * Originates from the GLCD library and adapted for Cosa Canvas. The
 * GLCD library was created by Michael Margolis and improved by Bill
 * Perry. 
 * @section Reference
 * https://code.google.com/p/glcd-arduino/source/browse/trunk/glcd/fonts/fixednums8x16.h
 * 
 */
class FixedNums8x16 : public GLCDFont {
public:
  /**
   * Construct fixed number font singleton.
   */
  FixedNums8x16() : GLCDFont(8, 15, '+', ':', bitmap) {}

private:
  static const uint8_t bitmap[] PROGMEM;
};

extern FixedNums8x16 fixednums8x16;

#endif
