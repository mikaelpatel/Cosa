/**
 * @file PCF8591.cpp
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

#include "PCF8591.hh"

bool
PCF8591::begin(uint8_t cntl)
{
  m_cntl = cntl;
  twi.acquire(this);
  twi.write(m_cntl);
  sample();
  return (true);
}

bool
PCF8591::convert(uint8_t value)
{
  twi.acquire(this);
  int count = twi.write(m_cntl, &value, 1);
  twi.release();
  return (count == (sizeof(m_cntl) + sizeof(value)));
}
