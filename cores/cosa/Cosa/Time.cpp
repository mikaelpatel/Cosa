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

const uint8_t time_t::days_in[] __PROGMEM = {
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

time_t::time_t(clock_t c, int8_t zone)
{
  c += zone * (int32_t) SECONDS_PER_HOUR;
  uint16_t dayno = c / SECONDS_PER_DAY;
  c -= dayno * (uint32_t) SECONDS_PER_DAY;
  
  day = weekday_for(dayno);

  uint16_t y = EPOCH_YEAR;
  for (;;) {
    uint16_t days = days_per( y );
    if (dayno < days) break;
    dayno -= days;
    y++;
  }
  bool leap_year = is_leap(y);
  y -= EPOCH_YEAR;
  while (y > 100)
    y -= 100;
  year = y;
// save 11uS
//  year = y%100;

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
// save 38uS
//  hours = (c % SECONDS_PER_DAY) / SECONDS_PER_HOUR;

  uint16_t c_ms;
  if (hours < 18) // save 16uS
    c_ms = (uint16_t) c - (hours * (uint16_t) SECONDS_PER_HOUR);
  else
    c_ms = c - (hours * (uint32_t)SECONDS_PER_HOUR);
  minutes = c_ms / SECONDS_PER_MINUTE;
  seconds = c_ms - (minutes * SECONDS_PER_MINUTE);

// save 96uS
//  minutes = (c % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
//  seconds = (c % SECONDS_PER_MINUTE);
}

time_t::operator clock_t() const
{
  clock_t s = days() * SECONDS_PER_DAY;
  if (hours < 18) // save 14uS
    s += hours * (uint16_t) SECONDS_PER_HOUR;
  else
    s += hours * (uint32_t) SECONDS_PER_HOUR;
  s += minutes * (uint16_t) SECONDS_PER_MINUTE;
  s += seconds;
  return s;
}
