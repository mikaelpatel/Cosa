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
 * Connect a 1-10 Mohm pullup resistor to digital pin D3.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Touch.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStram.hh"
#include "Cosa/IOStream/Driver/UART.hh"

IOStream cout(&uart);

class Counter : private Touch {
private:
  uint16_t m_count;
public:
  Counter(Board::DigitalPin pin) : Touch(pin), m_count(0) {}
  virtual void on_key_down() { cout << PSTR("count = ") << ++m_count << endl; }
};

Counter key(Board::D3);

void setup()
{
  uart.begin(9600);
  cout << PSTR("CosaTouch: started") << endl;
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  RTC::begin();
}

