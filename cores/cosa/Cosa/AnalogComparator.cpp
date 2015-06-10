/**
 * @file Cosa/AnalogComparator.cpp
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

#include "Cosa/AnalogComparator.hh"

AnalogComparator* AnalogComparator::s_comparator = NULL;

void
AnalogComparator::on_interrupt(uint16_t arg)
{
  Event::push(Event::CHANGE_TYPE, this, arg);
}

ISR(ANALOG_COMP_vect)
{
  if (UNLIKELY(AnalogComparator::s_comparator == NULL)) return;
  AnalogComparator::s_comparator->on_interrupt();
}
