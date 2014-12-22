/**
 * @file IconArduino64x64.hh
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
 */

#ifndef COSA_ICON_ICONARDUINO64X64_HH
#define COSA_ICON_ICONARDUINO64X64_HH

#include "Cosa/Canvas/IconGlyph.hh"

/**
 * Icon size 64x64.
 */
class IconArduino64x64 : public IconGlyph {
public:
  /**
   * Construct Arduino Icon (64x64) singleton.
   */
  IconArduino64x64() : IconGlyph(data[0], data[1], &data[2]) {}

private:
  static const uint8_t data[] PROGMEM;
};

extern IconArduino64x64 iconarduino64x64;

#endif
