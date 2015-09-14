/**
 * @file Cosa/RTC_Config.hh
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

#ifndef COSA_RTC_CONFIG_HH
#define COSA_RTC_CONFIG_HH

// RTC Timer Configuration
#define COUNT 250
#define PRESCALE 64
#define TIMER_MAX (COUNT - 1)
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK (COUNT * US_PER_TIMER_CYCLE)
#define MS_PER_TICK (US_PER_TICK / 1000)
#define US_DIRECT_EXPIRE (800 / I_CPU)
#define US_TIMER_EXPIRE (US_PER_TICK - 1)

#endif
