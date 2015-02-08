/**
 * @file config.h
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

#ifndef CONFIG_H
#define CONGIG_H

#include "Cosa/Types.h"

enum bias_t {
  NORMAL_BIAS,
  POSITIVE_BIAS,
  NEGATIVE_BIAS
} __attribute__((packed));

enum rate_t {
  OUTPUT_RATE_0_75_HZ,
  OUTPUT_RATE_1_5_HZ,
  OUTPUT_RATE_3_HZ,
  OUTPUT_RATE_7_5_HZ,
  OUTPUT_RATE_15_HZ,
  OUTPUT_RATE_30_HZ,
  OUTPUT_RATE_75_HZ
} __attribute__((packed));

enum avg_t {
  SAMPLES_AVG_1,
  SAMPLES_AVG_2,
  SAMPLES_AVG_4,
  SAMPLES_AVG_8,
} __attribute__((packed));

union config_t {
  uint8_t reg;
  struct {
    bias_t bias:2;
    rate_t rate:3;
    avg_t avg:3;
  };
};
#endif
