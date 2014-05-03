/**
 * @file Cosa/FixedPoint.hh
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

#ifndef COSA_FIXEDPOINT_HH
#define COSA_FIXEDPOINT_HH

#include "Cosa/Types.h"

/**
 * Simple class for fixed point number representation.
 * @param[in] POINT fixed point binary point.
 */
template <uint8_t POINT>
class FixedPoint {
public:
  /** 
   * Construct fixed point integer with given value and binary point.
   * @param[in] value fixed point binary number.
   */
  FixedPoint(int16_t value):
    m_integer(value >> POINT),
    m_fraction((value < 0 ? -value : value) & ~(-1 << POINT))
  {
  }

  /** 
   * Return integer part of fixed point number.
   * @return integer.
   */
  int16_t get_integer() 
  { 
    return (m_integer); 
  }

  /** 
   * Return unsigned fraction part of fixed point number.
   * @return fraction.
   */
  uint16_t get_fraction() 
  { 
    return (m_fraction); 
  }

  /** 
   * Return scaled unsigned fraction part of fixed point number.
   * The decimal scale is applied to the faction (1/2, 1/4...1/2**point).
   * @param[in] scale decimal range (1..n)
   * @return scaled fraction.
   */
  uint16_t get_fraction(uint8_t scale);

private:
  int16_t m_integer;
  uint16_t m_fraction;
};

template<uint8_t POINT>
uint16_t 
FixedPoint<POINT>::get_fraction(uint8_t scale)
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

#endif
