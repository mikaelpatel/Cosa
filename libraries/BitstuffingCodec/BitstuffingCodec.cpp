/**
 * @file BitstuffingCodec.cpp
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

#include "BitstuffingCodec.hh"

/*
 * Calculating the start symbol (5-bits per symbol):
 * 0xa, 0x1a => 01010.11010 => 11010.01010 => 11.0100.1010 => 0x34a
 */
const uint8_t BitstuffingCodec::s_preamble[] __PROGMEM = {
  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x1a
};
