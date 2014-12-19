/**
 * @file IconArduino96x32.hh
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

#ifndef COSA_ICON_ICONARDUINO96X32_HH
#define COSA_ICON_ICONARDUINO96X32_HH

#include "Cosa/Canvas/IconGlyph.hh"

/**
 * Icon size 96x32.
 */
class IconArduino96x32 : public IconGlyph {
public:
  /**
   * Construct Arduino Icon (96x32) singleton.
   */
  IconArduino96x32() : IconGlyph(data[0], data[1], &data[2]) {}

private:
  static const uint8_t data[] PROGMEM;
};

extern IconArduino96x32 iconarduino96x32;

#endif
