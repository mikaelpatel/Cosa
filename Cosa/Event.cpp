/**
 * @file Cosa/Event.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
 * Event data structure with type, source and value.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Event.h"
#ifndef NDEBUG
#include "Trace.h"
#endif

static Event event[Event::QUEUE_MAX];

Queue Event::queue(Event::QUEUE_MAX, sizeof(Event), event);

uint8_t
Event::push(int type, void* source, uint16_t value)
{
  Event event(type, source, value);
  return (queue.enqueue(&event));
}

void 
Event::print()
{
#ifndef NDEBUG
  Trace::print_P(PSTR("Event("));
  Trace::print(_type);
  Trace::print_P(PSTR(", "));
  Trace::print((uint16_t) _source, 16);
  Trace::print_P(PSTR(", "));
  Trace::print(_value);
  Trace::print_P(PSTR(")"));
#endif
}

void 
Event::println() 
{
#ifndef NDEBUG
  print();
  Trace::println();
#endif
}


