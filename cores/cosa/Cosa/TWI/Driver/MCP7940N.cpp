/**
 * @file Cosa/TWI/Driver/MCP7940N.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/TWI/Driver/MCP7940N.hh"

MCP7940N::AlarmInterrupt::AlarmInterrupt(Board::ExternalInterruptPin pin, 
					 MCP7940N* rtcc) :
  ExternalInterrupt(pin, ExternalInterrupt::ON_FALLING_MODE),
  m_rtcc(rtcc)
{
}

void
MCP7940N::AlarmInterrupt::on_interrupt(uint16_t arg)
{
  UNUSED(arg);
  if (m_rtcc == NULL) return;
  m_rtcc->on_alarm();
}

int
MCP7940N::read(void* regs, uint8_t size, uint8_t pos)
{
  if (!twi.begin(this)) return (-1);
  twi.write(pos);
  int count = twi.read(regs, size);
  twi.end();
  return (count);
}

int
MCP7940N::write(void* regs, uint8_t size, uint8_t pos)
{
  if (!twi.begin(this)) return (-1);
  int count = twi.write(pos, regs, size);
  twi.end();
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
  config.when = when;
  alarm.day = config.as_uint8;
  if (write(&alarm, sizeof(alarm_t), pos) != sizeof(alarm_t)) 
    return (false);

  // Update control flags and enable alarm interrupt and handler
  control_t cntrl;
  if (read(&cntrl, sizeof(cntrl), offsetof(rtcc_t,control)) != sizeof(cntrl))
    return (false);
  if (nr == 0) 
    cntrl.alm0 = 1; 
  else if (nr == 1) 
    cntrl.alm1 = 1; 
  if (write(&cntrl, sizeof(cntrl), offsetof(rtcc_t,control)) != sizeof(cntrl))
    return (false);
  m_alarm_irq.enable();
  return (true);
}

bool 
MCP7940N::is_alarm(uint8_t nr)
{
  // Map alarm number to offset
  uint8_t pos;
  if (nr == 0)
    pos = offsetof(rtcc_t,alarm0.day);
  else if (nr == 1)
    pos = offsetof(rtcc_t,alarm1.day);
  else return (false);

  // Read alarm information and copy configuration
  alarm_t::config_t config;
  if (read(&config, sizeof(config), pos) != sizeof(config)) return (false);
  if (!config.triggered) return (false);

  // Clear the alarm if triggered but leave enabled
  config.triggered = false;
  return (write(&config, sizeof(config), pos) == sizeof(config));
}

bool
MCP7940N::clear_alarm(uint8_t nr)
{
  // Update control flags; read, clear flag and write back
  control_t cntrl;
  if (read(&cntrl, sizeof(cntrl), offsetof(rtcc_t,control)) != sizeof(cntrl))
    return (false);
  if (nr == 0) 
    cntrl.alm0 = 0; 
  else if (nr == 1) 
    cntrl.alm1 = 0; 
  else return (false);
  if (write(&cntrl, sizeof(cntrl), offsetof(rtcc_t,control)) != sizeof(cntrl))
    return (false);

  // Check if interrupt handler should be disabled
  if (!cntrl.alm0 && !cntrl.alm1) m_alarm_irq.disable();
  return (true);
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
  outs << t.clock << ' '
       << bin << t.control.as_uint8 << ' '
       << t.calibration << ' '
       << t.alarm0 << ' '
       << t.alarm1 << ' ';
  return (outs);
}
