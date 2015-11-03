/**
 * @file TCS230.cpp
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

#include "TCS230.hh"
#include "Cosa/RTT.hh"

TCS230::TCS230(Board::ExternalInterruptPin out,
	       Board::DigitalPin s0,
	       Board::DigitalPin s1,
	       Board::DigitalPin s2,
	       Board::DigitalPin s3) :
  m_out(out),
  m_s0(s0, 1),
  m_s1(s1, 0),
  m_s2(s2, 1),
  m_s3(s3, 1)
{
}

void
TCS230::photodiode(Filter type)
{
  m_s2.set(type & 0x2);
  m_s3.set(type & 0x1);
}

void
TCS230::frequency_scaling(uint8_t percent)
{
  // Power-down on 0%
  if (percent == 0) {
    m_s0.low();
    m_s1.low();
  }

  // Scale 21..255 to 100%
  else if (percent > 20) {
    m_s0.high();
    m_s1.high();
  }

  // Scale 3..20 to 20%
  else if (percent > 2) {
    m_s0.high();
    m_s1.low();
  }

  // Scale 1..2 to 2%
  else {
    m_s0.low();
    m_s1.high();
  }
}

uint16_t
TCS230::sample(uint8_t ms)
{
  // Measure number of pulses of the given time period
  m_out.m_count = 0;
  uint32_t start = RTT::micros();
  m_out.enable();
  RTT::delay(ms);
  m_out.disable();
  uint32_t stop = RTT::micros();

  // Check for over-flow
  if (m_out.m_count == m_out.MAX) return (UINT16_MAX);

  // Scale with run-time
  uint32_t run = stop - start;
  return ((m_out.m_count * run) / (ms * 1000L));
}

TCS230::IRQPin::IRQPin(Board::ExternalInterruptPin pin) :
  ExternalInterrupt(pin, ExternalInterrupt::ON_RISING_MODE),
  m_count(0)
{
}

void
TCS230::IRQPin::on_interrupt(uint16_t arg)
{
  UNUSED(arg);
  m_count += 1;
  if (m_count < MAX) return;
  disable();
}


