/**
 * @file Font5x8.hh
 * @version 1.0
 *
 * @author contributed by jeditekunum
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_FONT5X8_HH
#define COSA_FONT5X8_HH

#include "Font.hh"

/**
 * Font size 5x8.
 */
class Font5x8 : public Font {
public:
  /**
   * Construct font (5x8) singleton.
   */
  Font5x8() :
    Font(width, height, first, last, bitmap, compression_type)
  {}

private:
  static const uint8_t bitmap[] PROGMEM;
  static const uint8_t compression_type;
  static const uint8_t width;
  static const uint8_t height;
  static const uint8_t first;
  static const uint8_t last;
} font5x8;

#include "Data/5x8.h"

#endif
