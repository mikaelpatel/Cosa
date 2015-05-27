/**
 * @file Cosa/Time.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

// Weekday numbers (1..7)
enum {
  SUNDAY = 1,
  MONDAY = 2,
  TUESDAY = 3,
  WEDNESDAY = 4,
  THURSDAY = 5,
  FRIDAY = 6,
  SATURDAY = 7
};

// NTP epoch year and weekday (Monday)
const uint16_t NTP_EPOCH_YEAR = 1900;
const uint8_t  NTP_EPOCH_WEEKDAY = MONDAY;

// POSIX epoch year and weekday (Thursday)
const uint16_t POSIX_EPOCH_YEAR = 1970;
const uint8_t  POSIX_EPOCH_WEEKDAY = THURSDAY;

// Y2K epoch year and weekday (Saturday)
const uint16_t Y2K_EPOCH_YEAR = 2000;
const uint8_t  Y2K_EPOCH_WEEKDAY = SATURDAY;

/**
 * Number of seconds elapsed since January 1 of the Epoch Year,
 * 00:00:00 +0000 (UTC).
 */
typedef uint32_t clock_t;

const uint32_t SECONDS_PER_DAY = 86400L;
const uint16_t SECONDS_PER_HOUR = 3600;
const uint8_t SECONDS_PER_MINUTE = 60;
const uint8_t DAYS_PER_WEEK = 7;

#if (ARDUINO > 150)
/**
 * User defined literal for hours to clock_t (seconds).
 * @param[in] h hours.
 * @return clock.
 */
constexpr clock_t operator "" _h(unsigned long long h)
{
  return (h * SECONDS_PER_HOUR);
}

/**
 * User defined literal for minutes to clock_t (seconds).
 * @param[in] min minutes.
 * @return clock.
 */
constexpr clock_t operator "" _min(unsigned long long min)
{
  return (min * SECONDS_PER_MINUTE);
}

/**
 * User defined literal for seconds to clock_t (seconds).
 * @param[in] s seconds.
 * @return clock.
 */
constexpr clock_t operator "" _s(unsigned long long s)
{
  return (s);
}
#endif

/**
 * Common date/time structure for real-time clocks. Data on some
 * devices is stored in BCD (DS1307/DS3231), although internal
 * representation is binary. Conversion methods are provided to
 * convert to/from the BCD representation. It is up the caller to keep
 * track of the representation.  All time_t methods (except
 * /to_binary/) expect the internal representation to be binary.
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
   * Convert time to binary representation (from BCD). Apply after
   * reading from a BCD device and before any calculation.
   */
  void to_binary()
    __attribute__((always_inline))
  {
    ::to_binary(&seconds, sizeof(time_t));
  }

  /**
   * Convert time to BCD representation (from binary). Apply after
   * setting new value and before writing to a BCD device.
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
   * Construct time record from seconds from the Epoch.
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
  uint16_t days() const;

  /**
   * Calculate day of the current year.
   * @return days from January 1, which is day zero.
   */
  uint16_t day_of_year() const;

  /**
   * Calculate 4-digit year from internal 2-digit year member.
   * @return 4-digit year.
   */
  uint16_t full_year() const
  {
    return full_year(year);
  }

  /**
   * Calculate 4-digit year from a 2-digit year
   * @param[in] year (4-digit).
   * @return true if /year/ is a leap year.
   */
  static uint16_t full_year(uint8_t year)
  {
    uint16_t y = year;

    if (y < pivot_year)
      y += 100 * (epoch_year()/100 + 1);
    else
      y += 100 * (epoch_year()/100);

    return y;
  }

  /**
   * Determine whether the current year is a leap year.
   * @returns true if the two-digit /year/ member is a leap year.
   */
  bool is_leap() const
  {
    return is_leap(full_year());
  }

  /**
   * Determine whether the 4-digit /year/ is a leap year.
   * @param[in] year (4-digit).
   * @return true if /year/ is a leap year.
   */
  static bool is_leap(uint16_t year)
  {
    if (year % 4) return false;
    uint16_t y = year % 400;
    return (y == 0) || ((y != 100) && (y != 200) && (y != 300));
  }

  /**
   * Calculate how many days are in the specified year.
   * @param[in] year (4-digit).
   * @return number of days.
   */
  static uint16_t days_per(uint16_t year)
  {
    return (365 + is_leap(year));
  }

  /**
   * Determine the day of the week for the specified day number
   * @param[in] day number as counted from January 1 of the epoch year.
   * @return weekday number 1..7, as for the /day/ member.
   */
  static uint8_t weekday_for(uint16_t dayno)
  {
    return ((dayno+epoch_weekday-1) % DAYS_PER_WEEK) + 1;
  }

  /**
   * Check that all members are set to a coherent date/time.
   * @return true if valid date/time.
   */
  bool is_valid() const
  {
    return
      ((year <= 99) &&
       (1 <= month) && (month <= 12) &&
       ((1 <= date) &&
	((date <= pgm_read_byte(&days_in[month])) ||
         ((month == 2) && is_leap() && (date == 29)))) &&
       (1 <= day) && (day <= 7) &&
       (hours <= 23) &&
       (minutes <= 59) &&
       (seconds <= 59));
  }

  /**
   * Set the epoch year for all time_t operations. Note that the pivot
   * year defaults to the epoch_year % 100. Valid years will be in the
   * range epoch_year..epoch_year+99. Selecting a different pivot year
   * will slide this range to the right.
   * @param[in] y epoch year to set.
   * See also /full_year/.
   */
  static void epoch_year(uint16_t y)
  {
    s_epoch_year = y;
    epoch_offset = s_epoch_year % 100;
    pivot_year = epoch_offset;
  }

  /**
   * Get the epoch year.
   * @return year.
   */
  static uint16_t epoch_year()
  {
    return (s_epoch_year);
  }

  static uint8_t epoch_weekday;

  /**
   * The pivot year determine the range of years WRT the epoch_year
   * For example, an epoch year of 2000 and a pivot year of 80 will
   * allow years in the range 1980 to 2079. Default 0 for Y2K_EPOCH.
   */
  static uint8_t pivot_year;

  /**
   * Use the current year for the epoch year. This will result in the
   * best performance, but dates/times before January 1 of this year
   * cannot be represented.
   */
  static void use_fastest_epoch();

  /**
   * Parse a character string and fill out members.
   * @param[in] s progmem character string with format "YYYY-MM-DD HH:MM:SS".
   * @return success.
   */
  bool parse(str_P s);

  /**
   * Days by month index 1..12 in program memory vector.
   */
  static const uint8_t days_in[] PROGMEM;

protected:
  static uint16_t s_epoch_year;
  static uint8_t epoch_offset;
} __attribute__((packed));

/**
 * Print the date/time to the given stream with the format
 * "YYYY-MM-DD HH:MM:SS".
 * @param[in] outs output stream.
 * @param[in] t time structure.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, const time_t& t);

#endif
