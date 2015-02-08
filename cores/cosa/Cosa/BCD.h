/**
 * @file Cosa/BCD.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * @section Description
 * Simple two digit BCD convertion functions.
 *
 * @section Limitations
 * Handles only two digit BCD numbers (0..99).
 *
 * @section References
 * [1] AVR204: BCD Arithmetics, Atmel Corporation, 0938B–AVR–01/03.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_BCD_H
#define COSA_BCD_H

#include "Cosa/Types.h"

/**
 * Convert given two digit BCD (0x00..0x99) to binary value (0..99).
 * @param[in] value to convert.
 * @return binary value.
 */
inline uint8_t to_binary(uint8_t value) __attribute__((always_inline));
inline uint8_t
to_binary(uint8_t value)
{
  uint8_t high = (value >> 4);
  uint8_t low = (value & 0x0f);
  return ((high << 3) + (high << 1) + low);
}

/**
 * Convert given binary value (0..99) to two digit BCD (0x00..0x99).
 * @param[in] value to convert.
 * @return BCD value.
 */
inline uint8_t to_bcd(uint8_t value)  __attribute__((always_inline));
inline uint8_t
to_bcd(uint8_t value)
{
  uint8_t res = 0;
  while (value > 9) {
    res += 0x10;
    value -= 10;
  }
  return (res + value);
}

/**
 * Convert to binary representation (from BCD) per byte (00..99).
 * @param[in] buf buffer to convert to binary.
 * @param[in] size number of bytes to convert.
 */
inline void
to_binary(uint8_t* buf, size_t size)
{
  for (uint8_t i = 0; i < size; i++)
    buf[i] = to_binary(buf[i]);
}

/**
 * Convert to BCD representation (from binary) per byte (00..99).
 * @param[in] buf buffer to convert to BCD.
 * @param[in] size number of bytes to convert.
 */
inline void
to_bcd(uint8_t* buf, size_t size)
{
  for (uint8_t i = 0; i < size; i++)
    buf[i] = to_bcd(buf[i]);
}

#endif

