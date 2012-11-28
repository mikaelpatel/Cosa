/**
 * @file Cosa/Button.h
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
 * Debounded Button abstraction. Sampled input pin.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Button.h"
#include "Cosa/Watchdog.h"

void
Button::attach(InputPin* pin)
{
  _pin = pin;
  _state = pin->is_set();
  set_event_handler(on_timeout);
  Watchdog::attach(this, SAMPLE_MS);
}

void 
Button::on_timeout(Thing* it, uint8_t type, uint16_t value)
{
  Button* button = (Button*) it;
  InputPin* pin = button->_pin;
  if (type != Event::TIMEOUT_TYPE || pin == 0) return;
  
  // Update the button state
  uint8_t old_state = button->_state;
  button->_state = pin->is_set();
  uint8_t new_state = button->_state;

  // If changed according to mode call the pin event handler
  if ((old_state != new_state) && 
      ((button->_mode == Button::ON_CHANGE_MODE) ||
       (new_state == button->_mode)))
    pin->on_event(Event::FALLING_TYPE + button->_mode, value);
}
