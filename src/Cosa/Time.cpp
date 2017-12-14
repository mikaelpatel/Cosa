/**
 * @file Cosa/Time.cpp
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

#include <stdlib.h>
#include "Time.hh"

IOStream& operator<<(IOStream& outs, const time_t& t)
{
  outs << time_t::full_year( t.year ) << '-';
  if (t.month < 10) outs << '0';
  outs << t.month << '-';
  if (t.date < 10) outs << '0';
  outs << t.date << ' ';
  if (t.hours < 10) outs << '0';
  outs << t.hours << ':';
  if (t.minutes < 10) outs << '0';
  outs << t.minutes << ':';
  if (t.seconds < 10) outs << '0';
  outs << t.seconds;
  return (outs);
}

bool
time_t::parse(str_P s)
{
  static size_t BUF_MAX = 32;
  char buf[BUF_MAX];
  strcpy_P(buf, s);
  char* sp = &buf[0];
  uint16_t value = strtoul(sp, &sp, 10);

  if (UNLIKELY(*sp != '-')) return false;
  year = value % 100;
  if (UNLIKELY(full_year() != value)) return false;

  value = strtoul(sp + 1, &sp, 10);
  if (UNLIKELY(*sp != '-')) return false;
  month = value;

  value = strtoul(sp + 1, &sp, 10);
  if (UNLIKELY(*sp != ' ')) return false;
  date = value;

  value = strtoul(sp + 1, &sp, 10);
  if (UNLIKELY(*sp != ':')) return false;
  hours = value;

  value = strtoul(sp + 1, &sp, 10);
  if (UNLIKELY(*sp != ':')) return false;
  minutes = value;

  value = strtoul(sp + 1, &sp, 10);
  if (UNLIKELY(*sp != 0)) return false;
  seconds = value;

  set_day();
  return (is_valid());
}

uint16_t time_t::s_epoch_year = Y2K_EPOCH_YEAR;
uint8_t time_t::epoch_offset = 0;
uint8_t time_t::epoch_weekday = Y2K_EPOCH_WEEKDAY;
uint8_t time_t::pivot_year = 0;

const uint8_t time_t::days_in[] __PROGMEM = {
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

time_t::time_t(clock_t c, int8_t zone)
{
  c += zone * (int32_t) SECONDS_PER_HOUR;
  uint16_t dayno = c / SECONDS_PER_DAY;
  c -= dayno * (uint32_t) SECONDS_PER_DAY;
  day = weekday_for(dayno);

  uint16_t y = epoch_year();
  for (;;) {
    uint16_t days = days_per(y);
    if (dayno < days) break;
    dayno -= days;
    y++;
  }
  bool leap_year = is_leap(y);
  y -= epoch_year();
  y += epoch_offset;
  while (y > 100)
    y -= 100;
  year = y;

  month = 1;
  for (;;) {
    uint8_t days = pgm_read_byte(&days_in[month]);
    if (leap_year && (month == 2)) days++;
    if (dayno < days) break;
    dayno -= days;
    month++;
  }
  date = dayno + 1;

  hours = c / SECONDS_PER_HOUR;

  uint16_t c_ms;
  if (hours < 18) // save 16uS
    c_ms = (uint16_t) c - (hours * (uint16_t) SECONDS_PER_HOUR);
  else
    c_ms = c - (hours * (uint32_t) SECONDS_PER_HOUR);
  minutes = c_ms / SECONDS_PER_MINUTE;
  seconds = c_ms - (minutes * SECONDS_PER_MINUTE);
}

time_t::operator clock_t() const
{
  clock_t c = days() * SECONDS_PER_DAY;
  if (hours < 18)
    c += hours * (uint16_t) SECONDS_PER_HOUR;
  else
    c += hours * (uint32_t) SECONDS_PER_HOUR;
  c += minutes * (uint16_t) SECONDS_PER_MINUTE;
  c += seconds;

  return (c);
}

uint16_t time_t::days() const
{
  uint16_t day_count = day_of_year();

  uint16_t y = full_year();
  while (y-- > epoch_year())
    day_count += days_per(y);

  return (day_count);
}

uint16_t time_t::day_of_year() const
{
  uint16_t dayno = date - 1;
  bool leap_year = is_leap();

  for (uint8_t m = 1; m < month; m++) {
    dayno += pgm_read_byte(&days_in[m]);
    if (leap_year && (m == 2)) dayno++;
  }

  return (dayno);
}

void time_t::use_fastest_epoch()
{
  // Figure out when we were compiled and use the year for a really
  // fast epoch_year. Format "MMM DD YYYY"
  const char* compile_date = (const char *) PSTR(__DATE__);
  uint16_t compile_year = 0;
  for (uint8_t i = 7; i < 11; i++)
    compile_year = compile_year*10 + (pgm_read_byte(&compile_date[i]) - '0');

  // Temporarily set a Y2K epoch so we can figure out the day for
  // January 1 of this year
  epoch_year(Y2K_EPOCH_YEAR);
  epoch_weekday = Y2K_EPOCH_WEEKDAY;
  time_t this_year(0);
  this_year.year = compile_year % 100;
  this_year.set_day();
  uint8_t compile_weekday = this_year.day;

  time_t::epoch_year(compile_year);
  time_t::epoch_weekday = compile_weekday;
  time_t::pivot_year = this_year.year;
}
