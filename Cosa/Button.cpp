/**
 * @file Cosa/Button.cpp
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
 * Debounded Button; Sampled input pin with pullup resistor. Uses a
 * watchdog timeout event (64 ms) for sampling and on change calls an
 * event action. Subclass Button and implement the virtual on_change()
 * method. Use the subclass for any state needed for the action
 * function.  
 *
 * @section Circuit
 * Connect button/switch from pin to ground. 
 *
 * @section Limitations
 * Button toggle faster than sample period may be missed.
 * 
 * @section See Also
 * The Button event handler requires the usage of an event
 * dispatch. See Event.hh. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Button.hh"
#include "Cosa/Watchdog.hh"

Button::Button(uint8_t pin, Mode mode) : 
  m_pin(pin, InputPin::PULLUP_MODE),
  m_state(0),
  m_mode(mode)
{
  // Set initial state and attach to watchdog timeout queue
  m_state = m_pin.is_set();
  Watchdog::attach(this, SAMPLE_MS);
}

void 
Button::on_event(uint8_t type, uint16_t value)
{
  // Skip all but timeout events
  if (type != Event::TIMEOUT_TYPE) return;
  
  // Update the button state
  uint8_t old_state = m_state;
  m_state = m_pin.is_set();
  uint8_t new_state = m_state;

  // If changed according to mode call the pin change handler
  if ((old_state != new_state) && 
      ((m_mode == ON_CHANGE_MODE) || (new_state == m_mode)))
    on_change(Event::FALLING_TYPE + m_mode);
}

void 
Button::on_change(uint8_t type)
{
  // Default change event function
}
