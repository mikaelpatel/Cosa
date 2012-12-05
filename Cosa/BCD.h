/**
 * @file Cosa/BCD.h
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
 * BCD convertion functions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BCD_H__
#define __COSA_BCD_H__

inline uint8_t
bcd_to_bin(uint8_t value)
{
  uint8_t high = (value >> 4);
  uint8_t low = (value & 0xf);
  return ((high << 3) + (high << 1) + low);
}

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

