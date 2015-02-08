/**
 * @file color16.h
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
 * @section Description
 * Evaluate different methods of bit-field access using macro, enum,
 * shift and mask, and bit-fields struct definitions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COLOR16_H
#define COLOR16_H

#include "Cosa/Types.h"

union color16_t {
  uint16_t rgb;
  struct {
    unsigned int blue:5;
    unsigned int green:6;
    unsigned int red:5;
  };
};

// Use bit-field struct
uint16_t color16a(uint8_t red, uint8_t green, uint8_t blue);

// Use shift-mask operations
uint16_t color16b(uint8_t red, uint8_t green, uint8_t blue);

#endif
