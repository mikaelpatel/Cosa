/**
 * @file CosaTWIslave.ino
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
 * Cosa demonstration of a TWI slave. Please see CosaTWImaster.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/TWI.h"
#include "Cosa/Pins.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Trace.h"

class TWIslave : public TWI {
private:
  static const uint8_t CMD_MAX = 8;
  static const uint8_t ADDR = 0xC05A;
  static void request_handler(Thing* it, uint8_t type, uint16_t value);
  friend void request_handler(Thing* it, uint8_t type, uint16_t value);
  void update();

public:
  TWIslave() : TWI() { set_event_handler(request_handler); }
  bool begin();
};

bool
TWIslave::begin() 
{ 
  _vec[0].buf = _buf;
  _vec[0].size = sizeof(_buf);
  return (TWI::begin(ADDR, this)); 
}

void
TWIslave::update()
{
  _vec[0].buf[1] = PINB;
  _vec[0].buf[2] = PINC;
  _vec[0].buf[3] = PIND;
  trace.print(_vec[0].buf, 4);
}

void
TWIslave::request_handler(Thing* it, uint8_t type, uint16_t value)
{
  TWIslave* twi = (TWIslave*) it;
  INFO("type = %d", type);
  twi->update();
}

// The TWI interface and slave instance
TWIslave slave;

// Use the builtin led for a heartbeat
OutputPin ledPin(13);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaTWISlave: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Start the TWI slave 
  slave.begin();
}

void loop()
{
  // Await for events to service
  Event event;
  Event::queue.await(&event);
  ledPin.toggle();
  event.dispatch();
  ledPin.toggle();
}
