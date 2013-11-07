/**
 * @file Cosa/TWI/Driver/ADXL345.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/ADXL345.hh"

bool 
ADXL345::begin()
{
  // Read device register and santity check
  uint8_t id = read(DEVID);
  if (id != ID) return (false);
  write(DATA_FORMAT, _BV(FULL_RES) | RANGE_16G);
  write(POWER_CTL, _BV(MEASURE) | _BV(SLEEP) | WAKEUP_8_HZ);
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
  twi.begin(this);
  twi.write((uint8_t) reg, &value, sizeof(value));
  twi.end();
}

void 
ADXL345::write(Register reg, void* buffer, uint8_t count)
{
  twi.begin(this);
  twi.write((uint8_t) reg, buffer, count);
  twi.end();
}

void 
ADXL345::read(Register reg, void* buffer, uint8_t count)
{
  twi.begin(this);
  twi.write((uint8_t) reg); 
  twi.read(buffer, count);
  twi.end();
}

void
ADXL345::calibrate()
{
  sample_t value;
  calibrate(0, 0, 0);
  sample(value);
  calibrate(-value.x/4, -value.y/4, -value.z/4);
}

IOStream& 
operator<<(IOStream& outs, ADXL345& accelerometer)
{
  ADXL345::sample_t value;
  accelerometer.sample(value);
  outs << PSTR("ADXL345(x = ") << value.x
       << PSTR(", y = ") << value.y
       << PSTR(", z = ") << value.z
       << PSTR(")");
  return (outs);
}

