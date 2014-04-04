/**
 * @file Cosa/AnalogPins.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#include "Cosa/AnalogPins.hh"

bool
AnalogPins::samples_request()
{
  m_next = 0;
  return (AnalogPin::sample_request(get_pin_at(m_next), m_reference));
}

void 
AnalogPins::on_interrupt(uint16_t value)
{
  sampling_pin = 0;
  m_buffer[m_next++] = value;
  if (m_next != m_count) {
    AnalogPin::sample_request(get_pin_at(m_next), m_reference);
  } 
  else {
    Event::push(Event::SAMPLE_COMPLETED_TYPE, this, value);
  }
}
