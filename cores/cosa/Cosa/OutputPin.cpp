/**
 * @file Cosa/OutputPin.cpp
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

#include "Cosa/OutputPin.hh"

void
OutputPin::write(uint8_t value, OutputPin& clk, Direction order) const
{
  uint8_t bits = CHARBITS;
  if (order == MSB_FIRST) {
    do {
      _write(value & 0x80);
      clk._toggle();
      value <<= 1;
      clk._toggle();
    } while (--bits);
  }
  else {
    do {
      _write(value & 0x01);
      clk._toggle();
      value >>= 1;
      clk._toggle();
    } while (--bits);
  }
}

void
OutputPin::write(uint16_t value, uint8_t bits, uint16_t us) const
{
  if (UNLIKELY(bits == 0)) return;
  synchronized {
    do {
      _write(value & 0x01);
      DELAY(us);
      value >>= 1;
    } while (--bits);
  }
}

