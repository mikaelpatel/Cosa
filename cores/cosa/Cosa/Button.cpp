/**
 * @file Cosa/Button.cpp
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

#include "Cosa/Button.hh"

void 
Button::on_event(uint8_t type, uint16_t value)
{
  // Skip all but timeout events
  if (type != Event::TIMEOUT_TYPE) return;
  
  // Update the button state
  uint8_t old_state = m_state;
  m_state = is_set();
  uint8_t new_state = m_state;

  // If changed according to mode call the pin change handler
  if ((old_state != new_state) && 
      ((MODE == ON_CHANGE_MODE) || (new_state == MODE)))
    on_change(Event::FALLING_TYPE + MODE);
}
