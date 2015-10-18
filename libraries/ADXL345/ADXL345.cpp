/**
 * @file ADXL345.cpp
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

#include "ADXL345.hh"

bool
ADXL345::begin()
{
  // Read device register and santity check
  uint8_t id = read(DEVID);
  if (UNLIKELY(id != ID)) return (false);

  // Data format
  write(DATA_FORMAT, _BV(INT_INVERT) | _BV(FULL_RES) | RANGE_16G);

  // Single and double tap detection
  write(THRESH_TAP, 0x40);
  write(DUR, 0x30);
  write(LATENT, 0x40);
  write(WINDOW, 0xFF);
  write(TAP_AXES, X | Y | Z);

  // Activity/inactivity detect
  write(THRESH_ACT, 6);
  write(THRESH_INACT, 3);
  write(TIME_INACT, 6);
  write(ACT_INACT_CTL, 0xff);

  // Free fall detect
  write(THRESH_FF, 0x05);
  write(TIME_FF, 0x14);

  // Power control
  write(POWER_CTL, _BV(AUTO_SLEEP) | _BV(MEASURE) | WAKEUP_2_HZ);

  // Interrupt enable
  write(INT_MAP, 0);
  write(INT_ENABLE, _BV(ACT) | _BV(SINGLE_TAP) | _BV(DOUBLE_TAP) | _BV(FREE_FALL));

  return (true);
}

bool
ADXL345::end()
{
  write(POWER_CTL, 0);
  return (true);
}

void
ADXL345::write(Register reg, uint8_t value)
{
  twi.acquire(this);
  twi.write((uint8_t) reg, &value, sizeof(value));
  twi.release();
}

void
ADXL345::write(Register reg, void* buffer, uint8_t count)
{
  twi.acquire(this);
  twi.write((uint8_t) reg, buffer, count);
  twi.release();
}

void
ADXL345::read(Register reg, void* buffer, uint8_t count)
{
  twi.acquire(this);
  twi.write((uint8_t) reg);
  twi.read(buffer, count);
  twi.release();
}

void
ADXL345::calibrate()
{
  sample_t value;
  calibrate(0, 0, 0);
  sample(value);
  calibrate(-value.x/4, -value.y/4, -value.z/4);
}

uint8_t
ADXL345::is_activity()
{
  uint8_t source = read(INT_SOURCE);
  if (source & _BV(INACT)) {
    uint8_t rate = read(BW_RATE);
    write(BW_RATE, rate | _BV(LOW_POWER));
  }
  if (source & _BV(ACT)) {
    uint8_t rate = read(BW_RATE);
    write(BW_RATE, rate & ~_BV(LOW_POWER));
  }
  return (source & 0x74);
}

IOStream&
operator<<(IOStream& outs, ADXL345& accelerometer)
{
  ADXL345::sample_t value;
  accelerometer.sample(value);
  outs << PSTR("ADXL345(x = ") << (value.x << 2)
       << PSTR(", y = ") << (value.y << 2)
       << PSTR(", z = ") << (value.z << 2)
       << PSTR(")");
  return (outs);
}

