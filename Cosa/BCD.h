/**
 * @file Cosa/BCD.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#ifndef __COSA_BCD_H__
#define __COSA_BCD_H__

/**
 * Convert given two digit BCD (0x00..0x99) to binary value (0..99).
 * @param[in] value to convert.
 * @return binary value.
 */
inline uint8_t
bcd_to_bin(uint8_t value)
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
inline uint8_t
bin_to_bcd(uint8_t value)
{
  uint8_t res = 0;
  while (value > 9) {
    res += 0x10;
    value -= 10;
  }
  return (res + value);
}

#endif

