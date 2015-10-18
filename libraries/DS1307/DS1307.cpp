/**
 * @file DS1307.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "DS1307.hh"

int
DS1307::read(void* ram, uint8_t size, uint8_t pos)
{
  twi.acquire(this);
  twi.write(pos);
  int count = twi.read(ram, size);
  twi.release();
  return (count);
}

int
DS1307::write(void* ram, uint8_t size, uint8_t pos)
{
  twi.acquire(this);
  int count = twi.write(pos, ram, size);
  twi.release();
  if (count > 0) count -= 1;
  return (count);
}


bool
DS1307::enable(Rate rs)
{
  control_t control;
  control.rs = rs;
  control.sqwe = 1;
  control.out = 1;
  uint8_t pos = offsetof(DS1307::timekeeper_t, control);
  int count = write(&control, sizeof(control), pos);
  return (count == sizeof(control));
}

bool
DS1307::disable()
{
  control_t control;
  uint8_t pos = offsetof(DS1307::timekeeper_t, control);
  int count = write(&control, sizeof(control), pos);
  return (count == sizeof(control));
}

