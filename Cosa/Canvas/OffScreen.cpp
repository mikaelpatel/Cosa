/**
 * @file Cosa/Canvas/OffScreen.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Off-screen canvas for drawing before copying to the canvas device.
 * Supports only monochrome, 1-bit, pixel in off-screen buffer. 
 * Minimum implementation; draw_pixel() only.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Canvas/OffScreen.hh"

bool 
OffScreen::begin()
{
  memset(m_bitmap, 0, m_count);
  return (1);
}

bool 
OffScreen::end()
{
  return (1);
}
