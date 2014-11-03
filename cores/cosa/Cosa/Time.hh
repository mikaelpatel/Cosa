/**
 * @file Cosa/Time.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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

#ifndef COSA_TIME_HH
#define COSA_TIME_HH

#include "Cosa/Types.h"
#include "Cosa/BCD.h"
#include "Cosa/IOStream.hh"

const uint16_t NTP_EPOCH_YEAR = 1900;
const uint8_t  NTP_EPOCH_WEEKDAY = 2;  // Monday

const uint16_t POSIX_EPOCH_YEAR = 1970;
const uint8_t  POSIX_EPOCH_WEEKDAY = 5;  // Thursday

const uint8_t PIVOT_YEAR = 0;

const uint16_t EPOCH_YEAR = 2000;
const uint8_t  EPOCH_WEEKDAY = 7;

/**
 * Number of seconds elapsed since January 1 of the Epoch Year, 00:00:00 +0000 (UTC).
 */

typedef uint32_t clock_t;

const uint32_t SECONDS_PER_DAY = 86400L;
const uint16_t SECONDS_PER_HOUR = 3600;
const uint8_t SECONDS_PER_MINUTE = 60;
const uint8_t DAYS_PER_WEEK = 7;

/**
 * Common date/time structure for real-time clocks. Data on some devices
 * is stored in BCD (DS1307/DS3231), although internal representation is binary.
 * Conversion methods are provided to convert to/from the BCD representation.
 * It is up the caller to keep track of the representation.  All time_t methods
 * (except /to_binary/) expect the internal representation to be binary.
 */
struct time_t {
  uint8_t seconds;		//!< 00-59 Seconds.
  uint8_t minutes;		//!< 00-59 Minutes.
  uint8_t hours;		//!< 00-23 Hours.
  uint8_t day;			//!< 01-07 Day.
  uint8_t date;			//!< 01-31 Date.
  uint8_t month;		//!< 01-12 Month.
  uint8_t year;			//!< 00-99 Year.

  /**
   * Convert time to binary representation (from BCD). 
   * Apply after reading from a BCD device and before any calculation.
   */
  void to_binary()
    __attribute__((always_inline))
  {
    ::to_binary(&seconds, sizeof(time_t));
  }

  /**
   * Convert time to BCD representation (from binary).
   * Apply after setting new value and before writing to a BCD device.
   */
  void to_bcd()
    __attribute__((always_inline))
  {
    ::to_bcd(&seconds, sizeof(time_t));
  }

  /**
   * Default constructor.
   */
  time_t() {}

  /**
   * Construct time record from seconds from NTP Epoch.
   * @param[in] c clock.
   * @param[in] zone time (hours adjustment from UTC).
   */
  time_t(clock_t c, int8_t zone = 0);

  /**
   * Convert time to clock representation.
   * @return seconds from epoch.
   */
  operator clock_t() const;

  /**
   * Set day member from time record.
   */
  void set_day()
  {
    day = weekday_for(days());
  }

  /**
   * Convert time to days.
   * @return days from January 1 of the epoch year.
   */
  uint16_t days() const
  {
    uint16_t day_count = day_of_year();

    uint16_t y = full_year();
    while (y-- > EPOCH_YEAR)
      day_count += days_per(y);
  // save 110uS
  //  day_count += (y-EPOCH_YEAR) * (SECONDS_PER_DAY * 365);
  //  while (y-- > EPOCH_YEAR)
  //    if (is_leap(y)) day_count++;

    return day_count;
  }

  /**
   * Calculate day of the current year.
   * @return days from January 1, which is day zero.
   */
  uint16_t day_of_year() const
  {
    uint16_t dayno = date-1;

    bool leap_year = is_leap();
    for (uint8_t m=1; m < month; m++) {
      dayno += pgm_read_byte(&days_in[m]);
      if (leap_year && (m == 2)) dayno++;
  // save 14uS    if ((m == 2) && is_leap()) dayno++;
    }

    return dayno;
  }

  /**
   * Calculate 4-digit year from internal 2-digit year member.
   * @return 4-digit year.
   */
  uint16_t full_year() const { return full_year(year); }

  /**
   * class method to calculate 4-digit year from a 2-digit year
   * @param[in] year (4-digit).
   * @return true if /year/ is a leap year.
   */
  static uint16_t full_year( uint8_t year )
  {
    uint16_t y = year;
    if (y < PIVOT_YEAR)
      y += 100 * (EPOCH_YEAR/100 + 1);
    else
      y += 100 * (EPOCH_YEAR/100);

    return y;
  }

  /**
   * Determine whether the current year is a leap year.
   * @returns true if the two-digit /year/ member is a leap year.
   */
  bool is_leap() const
  {
    return is_leap( full_year() );
  }

  /**
   * class method to determine whether the 4-digit /year/ is a leap year.
   * @param[in] year (4-digit).
   * @return true if /year/ is a leap year.
   */
  static bool is_leap(uint16_t year)
  {
    if (year % 4) return false;
    uint16_t y = year%400;
    return (y == 0) || ((y != 100) && (y != 200) && (y != 300));
//  save 20uS
//  return (!((year) % 4) && (((year) % 100) || (!((year) % 400))));
  }

  /**
   * class method to calculate how many days are in the specified year.
   * @param[in] year (4-digit).
   * @return number of days.
   */
  static uint16_t days_per(uint16_t year)
  {
    return (is_leap(year) ? 366 : 365);
  }

  /**
   * class method to determine the day of the week for the specified day number
   * @param[in] day number as counted from January 1 of the epoch year.
   * @return weekday number 1..7, as for the /day/ member.
   */
  static uint8_t weekday_for(uint16_t dayno)
  {
    return ((dayno+EPOCH_WEEKDAY-1) % DAYS_PER_WEEK) + 1;
  }

  static const uint8_t days_in[]; // month index is 1..12, PROGMEM
};

/**
 * Print the date/time to the given stream with the format (YYYY-MM-DD HH:MM:SS).
 * @param[in] outs output stream.
 * @param[in] t time structure.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, time_t& t);

#endif
