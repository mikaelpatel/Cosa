/**
 * @file Cosa/Power.cpp
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

#include "Cosa/Power.hh"

uint8_t Power::s_mode = SLEEP_MODE_IDLE;

#if defined(COSA_BROWN_OUT_DETECT) || !defined(sleep_bod_disable)
#define sleep_bod_disable()
#endif

void
Power::sleep(uint8_t mode)
{
  uint8_t saved = ADCSRA;
  ADCSRA = 0;
  if (mode == POWER_SLEEP_MODE) mode = s_mode;
  set_sleep_mode(mode);
  synchronized {
    sleep_enable();
    sleep_bod_disable();
  }
  sleep_cpu();
  sleep_disable();
  ADCSRA = saved;
}

