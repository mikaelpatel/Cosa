/**
 * @file Cosa/Power.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"

// Software disable low voltage detect (23 uA at 5 V)
// #define BOD_DISABLE

void
Power::sleep(uint8_t mode)
{
  set_sleep_mode(mode);
  sleep_enable();
#if defined(BOD_DISABLE)
  MCUCR = _BV(BODS) | _BV(BODSE);
  MCUCR = _BV(BODS);
#endif
  sleep_cpu();
  sleep_disable();
}
