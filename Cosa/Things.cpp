/**
 * @file Cosa/Things.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * The Cosa low level callback set handling.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Things.h"

void 
Things::broadcast(Thing* things, uint8_t type, uint16_t value)
{
  Thing* thing = things->get_succ(); 
  while (thing != things) {
    Thing* succ = thing->get_succ();
    thing->on_event(type, value);
    thing = succ;
  }
}

uint8_t 
Things::length()
{
  uint8_t res = 0;
  for (Thing* it = _succ; it != this; it = it->get_succ()) res++;
  return (res);
}


