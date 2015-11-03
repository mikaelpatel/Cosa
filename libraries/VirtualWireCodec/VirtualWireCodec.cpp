/**
 * @file VirtualWireCodec.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2008-2013, Mike McCauley (Author/VirtualWire)
 * Copyright (C) 2013-2015, Mikael Patel (Cosa C++ port and refactoring)
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

#include "VirtualWireCodec.hh"

const uint8_t VirtualWireCodec::s_symbols[] __PROGMEM = {
  0xd,  0xe,  0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c,
  0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34
};

/*
 * Calculating the start symbol (6-bits per symbol):
 * 0x2a, 0x2a => 10.1010, 10.1010 (preamble 6-bit).
 * 0x38, 0x2c => 10.1100, 11.1000 => 1011,0011.1000 => 0xb38
 */
const uint8_t VirtualWireCodec::s_preamble[] __PROGMEM = {
  0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x38, 0x2c
};

uint8_t
VirtualWireCodec::decode4(uint8_t symbol)
{
  symbol &= SYMBOL_MASK;
  // FIX: Binary search for better speed
  for (uint8_t i = 0; i < membersof(s_symbols); i++)
    if (symbol == pgm_read_byte(&s_symbols[i]))
      return (i);
  return (0);
}

