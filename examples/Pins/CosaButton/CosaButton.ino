/**
 * @file CosaButton.ino
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
 * Cosa demonstration of button abstraction.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Button.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Use the built-in led
OutputPin ledPin(13);

// Input button
class OnOffButton : public Button {
private:
  uint8_t m_count;
public:
  OnOffButton(uint8_t pin, Button::Mode mode) : 
    Button(pin, mode),  
    m_count(0)
  {
  }

  virtual void on_change(uint8_t type)
  {
    ledPin.toggle();
    m_count += 1;
    if (ledPin.is_set()) {
      INFO("%d: on", m_count);
    }
    else {
      INFO("%d: off", m_count);
    }
  }
};

OnOffButton onOff(7, Button::ON_FALLING_MODE);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaButton: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Button));

  // Start the watchdog ticks and push time events
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
}

void loop()
{
  // The basic event top loop
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
