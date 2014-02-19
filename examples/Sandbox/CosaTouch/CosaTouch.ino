/**
 * @file CosaTouch.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Demonstration of Cosa Touch capacitive sensor.
 *
 * @section Circuit
 * Connect a 1-10 Mohm pullup resistor to digital pin D3 and D4.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Touch.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"

IOStream cout(&uart);

class Key : private Touch {
private:
  static int16_t s_value;
  int8_t m_offset;
  virtual void on_touch() 
  { 
    s_value += m_offset;
    cout << PSTR("value = ") << s_value << endl; 
  }
public:
  Key(Board::DigitalPin pin, int8_t offset) : 
    Touch(pin), 
    m_offset(offset) 
  {}
  static void set_value(int16_t value)
  {
    s_value = value;
  }
  static int16_t get_value()
  {
    return (s_value);
  }
};
int16_t Key::s_value = 0;

Key upkey(Board::D3, 1);
Key downkey(Board::D4, -1);

void setup()
{
  uart.begin(9600);
  cout << PSTR("CosaTouch: started") << endl;
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  RTC::begin();
  Key::set_value(0);
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
