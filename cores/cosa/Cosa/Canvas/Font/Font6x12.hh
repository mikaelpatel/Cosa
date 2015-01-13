/**
 * @file Cosa/Canvas/Font/Font6x12.hh
 * @version 1.0
 *
 * @author contributed by jediunix
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

#ifndef COSA_FONT_FONT6X12_HH
#define COSA_FONT_FONT6X12_HH

#include "Cosa/Canvas/CompressedFont.hh"

/**
 * Font size 6x12.
 */
class Font6x12 : public CompressedFont {
public:
  /**
   * Construct font (6x12) singleton.
   */
  Font6x12() :
    CompressedFont(width, height, first, last,
                   compressed_bitmap, offsets, compression_type)
  {}

private:
  static const uint16_t offsets[] PROGMEM;
  static const uint8_t compressed_bitmap[] PROGMEM;
  static const uint8_t compression_type;
  static const uint8_t width;
  static const uint8_t height;
  static const uint8_t first;
  static const uint8_t last;
};

extern Font6x12 font6x12;

#endif
