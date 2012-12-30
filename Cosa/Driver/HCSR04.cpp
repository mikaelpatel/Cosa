/**
 * @file Cosa/Driver/HCSR04.cpp
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
 * Device driver for Ultrasonic range module HC-SR04.
 *
 * @section Circuit
 * Connect HC-SR04 module to echo and trigger pin, and VCC and
 * ground. 
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read 
 * from the device. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/HCSR04.hh"

bool 
HCSR04::read(uint16_t& distance)
{
  distance = 0;
  m_trigPin.pulse(10);
  uint16_t timeout = TIMEOUT;
  uint16_t count = 0;
  while (m_echoPin.is_clear() && timeout--);
  if (timeout == 0) return (0);
  synchronized {
    while (m_echoPin.is_set() && timeout--) count++;
  }
  if (timeout == 0) return (0);
  distance = (count * 10L) / COUNT_PER_CM;
  return (1);
}
