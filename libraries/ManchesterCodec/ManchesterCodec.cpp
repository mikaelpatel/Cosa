/**
 * @file ManchesterCodec.cpp
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

#include "ManchesterCodec.hh"

// Manchester encoder table 4 to 8 bits. In binary to show how it works
const uint8_t ManchesterCodec::s_symbols[] __PROGMEM = {
  0b10101010,
  0b10101001,
  0b10100110,
  0b10100101,
  0b10011010,
  0b10011001,
  0b10010110,
  0b10010101,
  0b01101010,
  0b01101001,
  0b01100110,
  0b01100101,
  0b01011010,
  0b01011001,
  0b01010110,
  0b01010101
};

uint8_t
ManchesterCodec::decode4(uint8_t symbol)
{
  uint8_t res = 0;
  if (symbol & 1) res |= 1;
  if (symbol & 4) res |= 2;
  if (symbol & 16) res |= 4;
  if (symbol & 64) res |= 8;
  return (res);
}

// Ethernet frame preamble and delimiter/start symbol
const uint8_t ManchesterCodec::s_preamble[] __PROGMEM = {
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x5d
};

