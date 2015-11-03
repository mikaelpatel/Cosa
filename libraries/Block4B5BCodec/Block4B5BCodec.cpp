/**
 * @file Block4B5BCodec.cpp
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

#include "Block4B5BCodec.hh"

/*
 * Calculating the start symbol JK (5-bits per symbol):
 * 0x18, 0x11 => 11000.10001 => 10001.11000 => 10.0011.1000 => 0x238
 */
const uint8_t Block4B5BCodec::s_preamble[] __PROGMEM = {
  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x18, 0x11
};

const uint8_t Block4B5BCodec::s_symbols[] __PROGMEM = {
  0b11110,
  0b01001,
  0b10100,
  0b10101,
  0b01010,
  0b01011,
  0b01110,
  0b01111,
  0b10010,
  0b10011,
  0b10110,
  0b10111,
  0b11010,
  0b11011,
  0b11100,
  0b11101
};

const uint8_t Block4B5BCodec::s_codes[] __PROGMEM = {
  0xff, //  0: 0b00000
  0xff, //  1: 0b00001
  0xff, //  2: 0b00010
  0xff, //  3: 0b00011
  0xff, //  4: 0b00100
  0xff, //  5: 0b00101
  0xff, //  6: 0b00110
  0xff, //  7: 0b00111

  0xff, //  8: 0b01000
  1,    //  9: 0b01001
  4,	// 10: 0b01010
  5,	// 11: 0b01011
  0xff, // 12: 0b01100
  0xff, // 13: 0b01101
  6,	// 14: 0b01110
  7,	// 15: 0b01111

  0xff, // 16: 0b10000
  0xff, // 17: 0b10001
  8,	// 18: 0b10010
  9,	// 19: 0b10011
  2,	// 20: 0b10100
  3,	// 21: 0b10101
  10,	// 22: 0b10110
  11,	// 23: 0b10111

  0xff, // 24: 0b11000
  0xff, // 25: 0b11001
  12,	// 26: 0b11010
  13,	// 27: 0b11011
  14,	// 28: 0b11100
  15,	// 29: 0b11101
  0,	// 30: 0b11110
  0xff	// 31: 0b11111
};


