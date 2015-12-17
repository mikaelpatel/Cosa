/**
 * @file PCF8563.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "PCF8563.hh"

int
PCF8563::read(void* regs, uint8_t size, uint8_t pos)
{
  twi.acquire(this);
  twi.write(pos);
  int count = twi.read(regs, size);
  twi.release();
  return (count);
}

int
PCF8563::write(void* regs, uint8_t size, uint8_t pos)
{
  twi.acquire(this);
  int count = twi.write(pos, regs, size);
  twi.release();
  return (count - 1);
}

bool
PCF8563::get_time(time_t& now)
{
  uint8_t pos = offsetof(rtcc_t,clock);
  if (read(&now, sizeof(now), pos) != sizeof(now)) return (false);
  // Use only relevant bits
  now.seconds &= 0x7f;
  now.minutes &= 0x7f;
  now.hours &= 0x3f;
  now.day &= 0x3f;
  now.date &= 0x07;
  now.month &= 0x0f;
  // Map to time struct
  uint8_t temp = now.day;
  now.day = now.date + 1;
  now.date = temp;
  return (true);
}

bool
PCF8563::set_time(time_t& now)
{
  // Clear alarm setting
  if (!clear_alarm()) return (false);
  // Map to time struct to real-time clock format
  uint8_t temp = now.day;
  now.day = now.date;
  now.date = temp - 1;
  // Set real-time clock
  uint8_t pos = offsetof(rtcc_t,clock);
  int res = write(&now, sizeof(now), pos);
  return (res == sizeof(now));
}

bool
PCF8563::get_alarm(alarm_t& alarm)
{
  // Read alarm setting
  uint8_t pos = offsetof(rtcc_t,alarm);
  int res = read(&alarm, sizeof(alarm_t), pos);
  if (res != sizeof(alarm_t)) return (false);
  // Check if week day needs adjusting
  if (alarm.day & alarm_t::DISABLE) return (true);
  alarm.day += 1;
  return (true);
}

bool
PCF8563::set_alarm(alarm_t& alarm)
{
  // Clear alarm flag
  control_status_2_t cs2;
  uint8_t pos = offsetof(rtcc_t,cs2);
  cs2.AIE = 1;
  int res = write(&cs2, sizeof(cs2), pos);
  if (res != sizeof(cs2)) return (false);
  // Check for alarm day adjust
  if ((alarm.day & alarm_t::DISABLE) == 0) alarm.day -= 1;
  // Write alarm setting
  pos = offsetof(rtcc_t,alarm);
  res = write(&alarm, sizeof(alarm_t), pos);
  return (res == sizeof(alarm_t));
}

bool
PCF8563::pending_alarm()
{
  // Read control register and check for alarm interrupt
  control_status_2_t cs2;
  uint8_t pos = offsetof(rtcc_t,cs2);
  int res = read(&cs2, sizeof(cs2), pos);
  if (res != sizeof(cs2)) return (false);
  return (cs2.AF);
}

bool
PCF8563::clear_alarm()
{
  // Read control register and clear alarm flags
  control_status_2_t cs2;
  uint8_t pos = offsetof(rtcc_t,cs2);
  int res = write(&cs2, sizeof(cs2), pos);
  if (res != sizeof(cs2)) return (false);
  // Clear alarm setting
  pos = offsetof(rtcc_t,alarm);
  alarm_t alarm;
  res = write(&alarm, sizeof(alarm_t), pos);
  return (res == sizeof(alarm));
}

IOStream& operator<<(IOStream& cout, PCF8563::alarm_t &alarm)
{
  if (alarm.day & PCF8563::alarm_t::DISABLE)
    cout << PSTR("XX");
  else
    cout << bcd << alarm.day;
  cout << ' ';
  if (alarm.date & PCF8563::alarm_t::DISABLE)
    cout << PSTR("DD");
  else
    cout << bcd << alarm.date;
  cout << ' ';
  if (alarm.hours & PCF8563::alarm_t::DISABLE)
    cout << PSTR("HH");
  else
    cout << bcd << alarm.hours;
  cout << ':';
  if (alarm.minutes & PCF8563::alarm_t::DISABLE)
    cout << PSTR("MM");
  else
    cout << bcd << alarm.minutes;
  return (cout);
}

