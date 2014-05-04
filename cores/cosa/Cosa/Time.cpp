/**
 * @file Cosa/Time.cpp
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

#include "Cosa/Time.hh"

IOStream& operator<<(IOStream& outs, time_t& t)
{
  outs << PSTR("20") << bcd << t.year << '-'
       << bcd << t.month << '-'
       << bcd << t.date << ' '
       << bcd << t.hours << ':'
       << bcd << t.minutes << ':'
       << bcd << t.seconds;
  return (outs);
}

IOStream& operator<<(IOStream& outs, clock_t& c)
{
  outs << (c / SECONDS_PER_DAY) << '.' << (c % SECONDS_PER_DAY);
  return (outs);
}

static const uint8_t days_in[] __PROGMEM = {
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static bool 
is_leap(uint16_t year)
{
  return (!((year) % 4) && (((year) % 100) || (!((year) % 400))));
}

static uint16_t
days_per(uint16_t year)
{
  return (is_leap(year) ? 366 : 365);
}

time_t::time_t(clock_t c, uint8_t zone)
{
  c += SECONDS_PER_HOUR * zone;
  uint16_t dayno = c / SECONDS_PER_DAY;
  day = (dayno % DAYS_PER_WEEK) + 1;
  year = 0;
  while (1) {
    uint16_t days = days_per(year);
    if (dayno < days) break;
    dayno -= days;
    year += 1;
  }
  year %= 100;
  month = 1;
  while (1) {
    uint8_t days = pgm_read_byte(&days_in[month]);
    if (is_leap(year) && (month == 2)) days += 1;
    if (dayno < days) break;
    dayno -= days;
    month += 1;
  }
  date = dayno + 1;
  hours = (c % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
  minutes = (c % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
  seconds = (c % SECONDS_PER_MINUTE);
  to_bcd();
}

time_t::operator clock_t()
{
  clock_t res;
  to_binary();
  res = year * (SECONDS_PER_DAY * 365);
  for (uint8_t y = 0; y < year; y++) 
    if (is_leap(y)) 
      res +=  SECONDS_PER_DAY;
  for (uint8_t m = 1; m < month; m++) {
    uint8_t days = pgm_read_byte(&days_in[m]);
    if (is_leap(year) && (m == 2)) days += 1;
    res += SECONDS_PER_DAY * days;
  }
  res += (date - 1) * SECONDS_PER_DAY;
  res += hours * SECONDS_PER_HOUR;
  res += minutes * SECONDS_PER_MINUTE;
  res += seconds;
  return (res);
}
