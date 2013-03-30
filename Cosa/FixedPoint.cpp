/**
 * @file Cosa/FixedPoint.cpp
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
 * Library for fixed point number representation.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FixedPoint.hh"

uint16_t 
FixedPoint::get_fraction(uint8_t scale)
{
  uint16_t half = 5;
  uint16_t result = 0;
  if (scale == 0) return (m_fraction);
  while (--scale) 
    half = (half << 3) + (half << 1);
  for (uint8_t bit = (1 << (POINT - 1)); bit; bit >>= 1, half >>= 1)
    if (bit & m_fraction)
      result += half;
  return (result);
}

