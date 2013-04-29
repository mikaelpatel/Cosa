/**
 * @file Cosa/Driver/HCSR04.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/Driver/HCSR04.hh"

bool 
HCSR04::read(uint16_t& distance)
{
  distance = 0;

  // Give the device a trigger pulse
  m_trigPin.pulse(10);
  uint16_t timeout = TIMEOUT;

  // Wait for the response
  while (m_echoPin.is_clear() && timeout--);
  if (timeout == 0) return (false);

  // Measure the lenght of the return pulse
  uint16_t count = 0;
  synchronized {
    while (m_echoPin.is_set() && timeout--) count++;
  }
  if (timeout == 0) return (false);

  // And calculate the distance in milli-meters
  distance = (count * 10L) / COUNT_PER_CM;
  return (true);
}

void 
HCSR04::on_event(uint8_t type, uint16_t value)
{
  uint16_t distance;

  // Read the distance and check if there was a change
  if (!read(distance) || (m_distance == distance)) return;

  // Save the new distance and call the change handler
  m_distance = distance;
  on_change(distance);
}
