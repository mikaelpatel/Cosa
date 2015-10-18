/**
 * @file L3G4200D.cpp
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

#include "L3G4200D.hh"

bool
L3G4200D::begin()
{
  ctrl_reg1_t reg;
  reg.Xen = 1;
  reg.Yen = 1;
  reg.Zen = 1;
  reg.PD = 1;
  write(CTRL_REG1, reg);
  return (true);
}

bool
L3G4200D::end()
{
  write(CTRL_REG1, 0);
  return (true);
}

void
L3G4200D::write(Register reg, uint8_t value)
{
  twi.acquire(this);
  twi.write((uint8_t) reg, &value, sizeof(value));
  twi.release();
}

void
L3G4200D::write(Register reg, void* buffer, uint8_t count)
{
  twi.acquire(this);
  twi.write((uint8_t) (reg | AUTO_INC), buffer, count);
  twi.release();
}

uint8_t
L3G4200D::read(Register reg)
{
  uint8_t res;
  twi.acquire(this);
  twi.write((uint8_t) reg);
  twi.read(&res, sizeof(res));
  twi.release();
  return (res);
}

void
L3G4200D::read(Register reg, void* buffer, uint8_t count)
{
  twi.acquire(this);
  twi.write((uint8_t) (reg | AUTO_INC));
  twi.read(buffer, count);
  twi.release();
}

IOStream&
operator<<(IOStream& outs, L3G4200D& gyroscope)
{
  L3G4200D::sample_t value;
  gyroscope.sample(value);
  outs << PSTR("L3G4200D(x = ") << value.x
       << PSTR(", y = ") << value.y
       << PSTR(", z = ") << value.z
       << PSTR(")");
  return (outs);
}

