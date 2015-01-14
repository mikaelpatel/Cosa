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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_CANVAS_FONT_SEGMENT32X50_HH
#define COSA_CANVAS_FONT_SEGMENT32X50_HH

#include "Cosa/Canvas/CompressedFont.hh"

/**
 * Bitmap font size 32x50, segments, fixed with font with 
 * numbers only.
 *
 * @section Acknowledgements
 * Font data originates from the UTFT library created by 
 * Henning Karlsen.
 */
class Segment32x50 : public CompressedFont {
public:
  /** 
   * Construct large segment font (32x50) singleton.
   */
  Segment32x50() :
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

extern Segment32x50 segment32x50;

#endif
