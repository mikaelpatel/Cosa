/**
 * @file Cosa/Math.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel.
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

#ifndef COSA_MATH_HH
#define COSA_MATH_HH

#include <math.h>
#include "Cosa/Types.h"

/**
 * Random number in given range (0..range). Max range value is
 * RAND_MAX(0x7fff) - 1.
 * @param[in] range value.
 * @return random number.
 */
inline uint16_t
rand(uint16_t range)
{
  return (rand() % (range + 1));
}

/**
 * Random number in given range (low..high). Max high value is
 * RAND_MAX(0x7fff).
 * @param[in] low range value.
 * @param[in] high range value.
 * @return random number.
 */
inline int16_t
rand(int16_t low, int16_t high)
{
  return (rand() % (high - low + 1) + low);
}

/**
 * Random number in given range (0..range-1). Max range value is
 * RANDOM_MAX(0x7fffffffL).
 * @param[in] range value.
 * @return random number.
 */
inline uint32_t
random(uint32_t range)
{
  return (random() % (range + 1));
}

/**
 * Random number in given range (low..high-1). Max high value is
 * RANDOM_MAX(0x7fffffff).
 * @param[in] low range value.
 * @param[in] high range value.
 * @return random number.
 */
inline int32_t
random(int32_t low, int32_t high)
{
  return (random(high - low + 1) + low);
}

/**
 * Calculate log(2) of the given value. The given template parameter
 * type should be unsigned.
 * @param[in] T unsigned integer type (uint8_t, uint16_t,..)
 * @param[in] value
 * @return log(2) > value
 */
template<class T>
inline uint8_t log2(T value)
{
  uint8_t res = 0;
  while (value != 0) {
    res += 1;
    value >>= 1;
  }
  return (res);
}

/**
 * Template map function for given class/data type.
 * @param[in] T class value to map.
 * @param[in] x value to map.
 * @param[in] in_min minimum value in input range.
 * @param[in] in_max maximum value in input range.
 * @param[in] out_min minimum value in output range.
 * @param[in] out_max maximum value in output range.
 * @return mapping
 */
template<class T, T in_min, T in_max, T out_min, T out_max>
T map(T x)
{
  static_assert(in_min < in_max, "bad range for map function");
  static_assert(out_min < out_max, "bad domain for map function");
  if (UNLIKELY(x < in_min)) return (out_min);
  if (UNLIKELY(x > in_max)) return (out_max);
  T range = in_max - in_min;
  T domain = out_max - out_min;
  return ((((x - in_min) * domain) / range) + out_min);
}

/**
 * Template constrain function for given class/data type.
 * @param[in] T class value to constrain.
 * @param[in] x value to constrain.
 * @param[in] low minimum value.
 * @param[in] high maximum value.
 * @return constrain
 */
template<class T, T low, T high>
T constrain(T x)
{
  static_assert(low < high, "bad range for contrain function");
  return (x < low ? low : (x > high ? high : x));
}

/**
 * Template within range check function for given
 * class/data type.
 * @param[in] T class value check range.
 * @param[in] x value to check.
 * @param[in] low minimum range value.
 * @param[in] high maximum range value.
 * @return bool
 */
template<class T, T low, T high>
bool is_within(T x)
{
  return (!(x < low || x > high));
}

#endif
