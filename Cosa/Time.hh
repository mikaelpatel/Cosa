/**
 * @file Cosa/Time.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TIME_HH__
#define __COSA_TIME_HH__

#include "Cosa/Types.h"
#include "Cosa/BCD.h"
#include "Cosa/IOStream.hh"

/**
 * Common date/time structure for real-time clocks. Data on devices
 * is stored in BCD (DS1307/DS3231).
 */
struct time_t {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint8_t year;

  /**
   * Convert time to binary representation (from BCD). 
   * Apply after reading from device and before any calculation.
   */
  void to_binary()
  {
    ::to_binary(&seconds, sizeof(time_t));
  }

  /**
   * Convert time to BCD representation (from binary).
   * Apply after setting new value and writing to the device.
   */
  void to_bcd()
  {
    ::to_bcd(&seconds, sizeof(time_t));
  }
};

/**
 * Print the date/time to the given stream with the format (YYYY-MM-DD
 * HH:MM:SS). The time structure values should be in BCD i.e. not
 * converted to binary.  
 * @param[in] outs output stream.
 * @param[in] t time structure.
 */
IOStream& operator<<(IOStream& outs, time_t& t);

#endif

