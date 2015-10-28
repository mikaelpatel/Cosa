/**
 * @file CosaTime.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Cosa Time tests with internal RTC. Show time zone handling.
 * Expected values for seconds and day of week were obtained
 * from http://http://www.timeanddate.com/date
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Clock.hh"
#include "Cosa/Time.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/UART.hh"

RTT::Clock wall;
IOStream cout(&uart);
clock_t start_time;

static str_P pf(bool b)
{
  return (b ? PSTR("PASS: ") : PSTR("FAIL: "));
}

static char pf2(bool b)
{
  return (b ? '\0' : '!');
}

static bool check(time_t &now,
		  clock_t expected,
		  bool pass = true,
		  uint8_t dow = 8,
		  uint16_t days = 0)
{
  bool ok = pass;

  now.set_day();
  if ((dow != 8) && (dow != now.day)) {
    cout << pf(!pass) << pf2(pass) << now
	 << PSTR(" day of week ") << now.day
	 << PSTR(" != ") << dow;
    ok = !pass;

  } else if ((dow != 8) && (days != now.days())) {
    cout << pf(!pass) << pf2(pass) << now
	 << PSTR(" days since epoch ") << now.days()
	 << PSTR(" != ") << days;
    ok = !pass;

  } else {
    clock_t s = now;
    if (s != expected) {
      cout << pf(!pass) << pf2(pass) << now << ' '
	   << s << PSTR(" != ") << expected;
      ok = !pass;

    } else {
      cout << pf(pass) << now << PSTR(" == ") << expected;
    }
  }

  if (dow != 8)
    cout << PSTR(", day ") << now.day << PSTR(", days = ") << now.days();

  if (!ok) {
    cout << endl;
    return (ok);
  }

  const uint16_t imax = F_CPU / 16000UL;

  // Get some timings
  cout.flush();
  uint32_t us = RTT::micros();
  clock_t c;
  for (uint16_t i = 0; i < imax; i++) c = now;
  uint32_t elapsed_us = RTT::micros() - us;
  cout << PSTR(", to/from in ") << elapsed_us/imax << PSTR("us/");
  cout.flush();

  time_t test;
  us = RTT::micros();
  for (uint16_t i = 0; i < imax; i++) test = c;
  elapsed_us = RTT::micros() - us;
  cout << elapsed_us/imax << PSTR("us");

  // Check the conversion back to time_t
  if (test != now) {
    cout << endl << pf(!pass) << pf2(pass)
	 << PSTR("Reconverted ") << test << PSTR(" != ") << now;
    ok = !pass;
  }
  cout << endl;
  return (ok);
}

static int16_t passes = 0;
static int16_t fails = 0;

static void check(str_P c,
		  clock_t expected,
		  bool pass = true,
		  uint8_t dow = 8,
		  uint16_t days = 0)
{
  bool ok = pass;
  time_t now;

  if (!now.parse(c)) {
    cout << pf(!pass) << pf2(pass)
	 << PSTR("Parsing \"") << c << '"'
	 << endl;
    ok = !pass;

  } else if (!now.is_valid()) {
    cout << pf(!pass) << pf2(pass)
	 << PSTR("Valid date ") << now
	 << PSTR(" from \"") << c << '"'
	 << endl;
    ok = !pass;

  } else
    check(now, expected, pass, dow, days);

  if (ok)
    passes++;
  else
    fails++;
}

static void show_epoch()
{
  cout << PSTR("Epoch year ") << time_t::epoch_year()
       << PSTR(", weekday ") << time_t::epoch_weekday
       << PSTR(", pivot year ") << time_t::pivot_year << endl;
}

void setup()
{
  RTT::begin();
  Watchdog::begin();
  uart.begin(9600);
  cout << PSTR("CosaTime: started") << endl;

  cout << PSTR("Validity Tests") << endl;
  time_t::epoch_year(Y2K_EPOCH_YEAR);
  time_t::epoch_weekday = Y2K_EPOCH_WEEKDAY;
  show_epoch();
#if (ARDUINO > 150)
  cout << 7200_s << endl;
  cout << 120_min << endl;
  cout << 2_h << endl;
#endif
  check(PSTR("2089-06-04 16:45:29"), 2822057129UL, true, 7, 32662);
  check(PSTR("0100-01-01 00:00:00"), 0UL, false);
  check(PSTR("2000-00-01 00:00:00"), 0UL, false);
  check(PSTR("2001-10-00 00:00:00"), 0UL, false);
  check(PSTR("2000-11-31 00:00:00"), 0UL, false);
  check(PSTR("2005-01-01 24:00:00"), 0UL, false);
  check(PSTR("2010-03-01 23:60:00"), 0UL, false);
  check(PSTR("2011-04-01 22:21:60"), 0UL, false);
  check(PSTR("2014-02-29 00:00:00"), 0UL, false);
  cout << endl;

  cout << PSTR("NTP Tests") << endl;
  time_t::epoch_year( NTP_EPOCH_YEAR );
  time_t::epoch_weekday = NTP_EPOCH_WEEKDAY;
  time_t::pivot_year = 2; // 1902..2001 range
  show_epoch();
  check(PSTR("1902-12-31 23:59:55"), 94607995UL, true, 4, 1094);
  check(PSTR("1904-02-28 23:59:59"), 131241599UL, true, 1, 1518);
  check(PSTR("1904-02-29 00:00:00"), 131241600UL, true, 2, 1519);
  check(PSTR("1999-02-26 06:40:00"), 3129000000UL, true, 6, 36215);
  check(PSTR("1999-12-31 23:59:59"), 3155673599UL, true, 6, 36523);
  check(PSTR("2000-01-01 00:00:00"), 3155673600UL, true, 7, 36524);
  check(PSTR("2000-02-28 23:59:59"), 3160771199UL, true, 2, 36582);
  check(PSTR("2000-03-01 00:00:00"), 3160857600UL, true, 4, 36584);
  check(PSTR("2001-11-26 12:34:56"), 3215766896UL, true, 2, 37219);
  cout << endl;

  cout << PSTR("POSIX Tests") << endl;
  time_t::epoch_year(POSIX_EPOCH_YEAR);
  time_t::epoch_weekday = POSIX_EPOCH_WEEKDAY;
  time_t::pivot_year = 70; // 1970..2069 range
  show_epoch();
  check(PSTR("1972-12-31 23:59:55"), 94694395UL, true, 1, 1095);
  check(PSTR("1984-02-28 23:59:59"), 446860799UL, true, 3, 5171);
  check(PSTR("1984-02-29 00:00:00"), 446860800UL, true, 4, 5172);
  check(PSTR("2001-09-09 01:46:40"), 1000000000UL, true, 1, 11574);
  check(PSTR("1999-12-31 23:59:59"), 946684799UL, true, 6, 10956);
  check(PSTR("2000-01-01 00:00:00"), 946684800UL, true, 7, 10957);
  check(PSTR("2000-02-28 23:59:59"), 951782399UL, true, 2, 11015);
  check(PSTR("2000-03-01 00:00:00"), 951868800UL, true, 4, 11017);
  check(PSTR("2069-12-31 23:59:59"), 3155759999UL, true, 3, 36524);
  cout << endl;

  cout << PSTR("Fastest epoch Tests") << endl;
  time_t::use_fastest_epoch();
  show_epoch();
  time_t this_year(0);
  check(this_year, 0UL, true, time_t::epoch_weekday, 0);

  this_year.month = 2;
  uint16_t days = 31;
  check(this_year, days*SECONDS_PER_DAY, true, (time_t::epoch_weekday+days-1)%7 + 1, days);

  this_year.date = 28;
  days += 28-1;
  check(this_year, days*SECONDS_PER_DAY, true, (time_t::epoch_weekday+days-1)%7 + 1, days);

  this_year.month = 3;
  this_year.date = 1;
  days++;
  if ((this_year.year % 4) == 0) days++;
  check(this_year, days*SECONDS_PER_DAY, true, (time_t::epoch_weekday+days-1)%7 + 1, days);

  this_year.month = 12;
  this_year.date = 31;
  this_year.hours = 0;
  this_year.minutes = 0;
  this_year.seconds = 0;
  days = 365-1;
  if ((this_year.year % 4) == 0) days++;
  check(this_year, days*SECONDS_PER_DAY, true, (time_t::epoch_weekday+days-1)%7 + 1, days);
  cout << endl;

  // Set the RTT to a start time
  time_t now(0);
  now.seconds = 45;
  now.minutes = 59;
  now.hours = 23;
  now.date = 31;
  now.month = 12;
  start_time = now;
  cout << PSTR("Setting RTT to ") << now
       << PSTR(" (") << start_time << PSTR(" seconds)")
       << endl;
  wall.time(start_time);
}

void loop()
{
  // Read internal RTT time and create time for time zones
  clock_t clock = wall.time();
  time_t se(clock, 2);
  time_t utc(clock);
  time_t us(clock, -4);

  // Print seconds since epoch and time zones
  clock_t s = clock - start_time;
  cout << s << ':'
       << PSTR("se=") << se << ','
       << PSTR("utc=") << utc << ','
       << PSTR("us=") << us << ',';

  // Take a nap until seconds update. Count number of yields
  uint8_t cnt = 0;
  while (clock == wall.time()) {
    cnt += 1;
    yield();
  }

  // Print number of yields
  cout << PSTR("cnt=") << cnt << endl;

  if (s >= 20) {
    cout << endl;
    cout << PSTR("Tests completed:") << endl;
    cout << ' ' << passes << PSTR(" passed.") << endl;
    cout << ' ' << fails << PSTR(" failed.") << endl;
    for (;;) ;
  }
}
