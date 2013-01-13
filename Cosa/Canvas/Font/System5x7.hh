/**
 * @file Cosa/Canvas/Font/System5x7.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Bitmap font size 5x7.
 *
 * @section Acknowledgement
 * Inspired by graphics library by ladyada/adafruit.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CANVAS_FONT_SYSTEM5X7_HH__
#define __COSA_CANVAS_FONT_SYSTEM5X7_HH__

#include "Cosa/Canvas/Font.hh"

class System5x7 : public Font {
private:
  static const uint8_t bitmap[] PROGMEM;

public:
  System5x7() : Font(5, 8, bitmap) {}
};

extern System5x7 system5x7;

#endif
