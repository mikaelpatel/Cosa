/**
 * @file CosaKeypad.ino
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
 * Cosa demonstration of DFRobot LCD Keypad shield handler;
 * Simple trace of callback, on_key(), with printout to the LCD.
 *
 * @section Circuit
 * Requires a DFRobot LCD Keypad shield with 16x2 HD44780 LCD
 * and six buttons.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Keypad.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/LCD/Driver/HD44780.hh"

HD44780::Port port;
HD44780 lcd(&port);
IOStream cout(&lcd);

class KeypadTrace : public DFRobotLCDKeypad {
public:
  KeypadTrace() : DFRobotLCDKeypad() {}
  void trace(const char* msg, uint8_t nr);
  virtual void on_key_down(uint8_t nr) { trace(PSTR("down"), nr); }
  virtual void on_key_up(uint8_t nr) { trace(PSTR("up"), nr); }
};

void 
KeypadTrace::trace(const char* msg, uint8_t nr)
{
  cout << clear;
  switch (nr) {
  case NO_KEY: 
    cout << PSTR("NO_KEY"); 
    break;
  case SELECT_KEY: 
    cout << PSTR("SELECT_KEY"); 
    break;
  case LEFT_KEY: 
    cout << PSTR("LEFT_KEY"); 
    break;
  case DOWN_KEY: 
    cout << PSTR("DOWN_KEY"); 
    break;
  case UP_KEY: 
    cout << PSTR("UP_KEY"); 
    break;
  case RIGHT_KEY: 
    cout << PSTR("RIGHT_KEY"); 
    break;
  }
  cout << ' ' << msg << endl << PSTR("key = ") << nr;
}

KeypadTrace keypad;

void setup()
{
   Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
   lcd.begin();
   cout << PSTR("CosaKeypad: started");
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}

