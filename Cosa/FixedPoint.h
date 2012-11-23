/**
 * @file Cosa/FixedPoint.h
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

#ifndef __COSA_FIXEDPOINT_H__
#define __COSA_FIXEDPOINT_H__

#include "Cosa/Types.h"

class FixedPoint {
private:
  int16_t _integer;
  uint16_t _fraction;
  uint8_t _point;
public:
  /** 
   * Construct fixed point integer with given value and binary point.
   * @param[in] value fixed point binary number.
   * @param[in] point fixed point binary point.
   */
  FixedPoint(int16_t value, uint8_t point):
    _integer(value >> point),
    _fraction((value < 0 ? -value : value) & ~(-1 << point)),
    _point(point)
  {
  }

  /** 
   * Return integer part of fixed point number.
   * @return integer.
   */
  int16_t get_integer() { return (_integer); }

  /** 
   * Return unsigned fraction part of fixed point number.
   * @return fraction.
   */
  uint16_t get_fraction() { return (_fraction); }

  /** 
   * Return scaled unsigned fraction part of fixed point number.
   * The decimal scale is applied to the faction (1/2, 1/4...1/2**point).
   * @param[in] scale decimal range (1..n)
   * @return scaled fraction.
   */
  uint16_t get_fraction(uint8_t scale);
};

#endif
