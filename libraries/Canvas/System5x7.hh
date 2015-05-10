/**
 * @file System5x7.hh
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

#ifndef COSA_SYSTEM5X7_HH
#define COSA_SYSTEM5X7_HH

#include "Font.hh"

/**
 * Bitmap system font size 5x7. Full ASCII table (0..127).
 *
 * @section Acknowledgements
 * Inspired by graphics library by ladyada/adafruit.
 */
class System5x7 : public Font {
public:
  /**
   * Construct system font (5x7) singleton.
   */
  System5x7() :
    Font(width, height, first, last, bitmap)
  {}

private:
  static const uint8_t bitmap[] PROGMEM;
  static const uint8_t width;
  static const uint8_t height;
  static const uint8_t first;
  static const uint8_t last;
};

extern System5x7 system5x7;

#endif

