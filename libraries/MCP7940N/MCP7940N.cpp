/**
 * @file MCP7940N.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "MCP7940N.hh"

MCP7940N::AlarmInterrupt::AlarmInterrupt(Board::ExternalInterruptPin pin) :
  ExternalInterrupt(pin, ExternalInterrupt::ON_RISING_MODE),
  m_triggered(false)
{
}

void
MCP7940N::AlarmInterrupt::on_interrupt(uint16_t arg)
{
  UNUSED(arg);
  m_triggered = true;
}

int
MCP7940N::read(void* regs, uint8_t size, uint8_t pos)
{
  twi.acquire(this);
  twi.write(pos);
  int count = twi.read(regs, size);
  twi.release();
  return (count);
}

int
MCP7940N::write(void* regs, uint8_t size, uint8_t pos)
{
  twi.acquire(this);
  int count = twi.write(pos, regs, size);
  twi.release();
  return (count - 1);
}

bool
MCP7940N::get_time(time_t& now)
{
  if (read(&now, sizeof(now)) != sizeof(now)) return (false);
  now.seconds &= 0x7f;
  now.day &= 0x07;
  now.month &= 0x1f;
  return (true);
}

bool
MCP7940N::set_time(time_t& now)
{
  now.seconds |= 0x80;
  int res = write(&now, sizeof(now));
  now.seconds &= 0x7f;
  return (res == sizeof(now));
}

bool
MCP7940N::get_alarm(uint8_t nr, time_t& alarm, uint8_t& when)
{
  // Map alarm number to offset
  uint8_t pos;
  if (nr == 0)
    pos = offsetof(rtcc_t,alarm0);
  else if (nr == 1)
    pos = offsetof(rtcc_t,alarm1);
  else return (false);

  // Read alarm information and copy configuration
  if (read(&alarm, sizeof(alarm_t), pos) != sizeof(alarm_t)) return (false);
  when = alarm_t::config_t(alarm.day).when;
  alarm.day &= 0x7;
  return (true);
}

bool
MCP7940N::set_alarm(uint8_t nr, time_t& alarm, uint8_t when)
{
  // Map alarm number to offset
  uint8_t pos;
  if (nr == 0)
    pos = offsetof(rtcc_t,alarm0);
  else if (nr == 1)
    pos = offsetof(rtcc_t,alarm1);
  else return (false);

  // Create configuration and write alarm information
  alarm_t::config_t config(alarm.day);
  config.polarity = 1;
  config.when = when;
  alarm.day = config.as_uint8;
  if (write(&alarm, sizeof(alarm_t), pos) != sizeof(alarm_t)) return (false);

  // Update control flags and enable alarm interrupt and handler
  control_t cntrl;
  pos = offsetof(rtcc_t,control);
  if (read(&cntrl, sizeof(cntrl), pos) != sizeof(cntrl)) return (false);
  if (nr == 0)
    cntrl.alm0en = 1;
  else if (nr == 1)
    cntrl.alm1en = 1;
  if (write(&cntrl, sizeof(cntrl), pos) != sizeof(cntrl)) return (false);
  m_alarm_irq.enable();
  return (true);
}

uint8_t
MCP7940N::pending_alarm()
{
  // Check if an interrupt has been received
  alarm_t::config_t config;
  uint8_t pos;
  int res = 0;
  if (!m_alarm_irq.m_triggered) return (0);
  m_alarm_irq.m_triggered = false;

  // Read alarm 0 configuration. Check if alarm is triggered
  pos = offsetof(rtcc_t,alarm0.day);
  if (read(&config, sizeof(config), pos) != sizeof(config)) return (0);
  if (config.triggered) res |= 0x01;

  // Read alarm 1 configuration. Check if alarm is triggered
  pos = offsetof(rtcc_t,alarm1.day);
  if (read(&config, sizeof(config), pos) != sizeof(config)) return (0);
  if (config.triggered) res |= 0x02;
  return (res);
}

bool
MCP7940N::clear_alarm(uint8_t nr)
{
  // Update control flags; read, clear flag and write back
  control_t cntrl;
  uint8_t pos = offsetof(rtcc_t,control);
  if (read(&cntrl, sizeof(cntrl), pos) != sizeof(cntrl)) return (false);
  if (nr == 0)
    cntrl.alm0en = 0;
  else if (nr == 1)
    cntrl.alm1en = 0;
  else return (false);
  if (write(&cntrl, sizeof(cntrl), pos) != sizeof(cntrl)) return (false);

  // Check if interrupt handler should be disabled
  if (!cntrl.alm0en && !cntrl.alm1en) m_alarm_irq.disable();
  return (true);
}

bool
MCP7940N::square_wave(bool flag)
{
  control_t cntrl;
  uint8_t pos = offsetof(rtcc_t,control);
  if (read(&cntrl, sizeof(cntrl), pos) != sizeof(cntrl)) return (false);
  cntrl.sqwen = flag;
  return (write(&cntrl, sizeof(cntrl), pos) == sizeof(cntrl));
}

IOStream& operator<<(IOStream& outs, MCP7940N::alarm_t& t)
{
  outs << bcd << t.month << '-'
       << bcd << t.date << ' '
       << bcd << t.hours << ':'
       << bcd << t.minutes << ':'
       << bcd << t.seconds;
  return (outs);
}

IOStream& operator<<(IOStream& outs, MCP7940N::rtcc_t& t)
{
  t.clock.to_binary();
  outs << t.clock << ' '
       << bin << t.control.as_uint8 << ' '
       << t.calibration << ' '
       << t.alarm0 << ' '
       << t.alarm1 << ' ';
  t.clock.to_bcd();
  return (outs);
}
