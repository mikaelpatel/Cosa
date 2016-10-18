/**
 * @file Cosa/Pin.cpp
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

#include "Cosa/Pin.hh"
#include "Cosa/OutputPin.hh"

uint8_t
Pin::read(OutputPin& clk, Direction order) const
{
  uint8_t value = 0;
  uint8_t bits = CHARBITS;
  if (order == MSB_FIRST) {
    do {
      value <<= 1;
      if (is_set()) value |= 0x01;
      clk.toggle();
      clk.toggle();
    } while (--bits);
  }
  else {
    do {
      value >>= 1;
      if (is_set()) value |= 0x80;
      clk.toggle();
      clk.toggle();
    } while (--bits);
  }
  return (value);
}
