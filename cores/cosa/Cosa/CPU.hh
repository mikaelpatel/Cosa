/**
 * @file Cosa/CPU.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_CPU_HH
#define COSA_CPU_HH

#include "Cosa/Types.h"

/**
 * Processor frequency scaling and support functions.
 */
class CPU {
public:
  /**
   * Scale the clock frequency according to the give prescale
   * factor (0..4 for prescale pow2(factor) 1..16). Returns
   * previous prescale factor.
   * @param[in] prescale division factor (0..4).
   * @return previous prescale factor.
   */
  static uint8_t clock_prescale(uint8_t factor)
  {
    if (UNLIKELY(factor > 4)) factor = 4;
    uint8_t res = CLKPR;
    synchronized {
      CLKPR = _BV(CLKPCE);
      CLKPR = factor;
    }
    return (res);
  }

  /**
   * Returns current processor clock frequency. May be used
   * instead of F_CPU to adjust after prescaling.
   * @return processor clock frequency
   */
  static uint32_t clock_freq()
  {
    return (F_CPU >> CLKPR);
  }

private:
  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  CPU() {}
};

#endif
