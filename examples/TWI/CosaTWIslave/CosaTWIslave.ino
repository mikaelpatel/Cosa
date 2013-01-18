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

#include "Cosa/TWI.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"

// A simple TWI slave device
class TWIslave : public TWI::Device {
private:
  // Address of device on TWI bus
  static const uint8_t ADDR = 0xC05A;

  // Buffer for request and respons
  static const uint8_t BUF_MAX = 4;
  uint8_t m_buf[BUF_MAX];

public:
  // Request handler; events from incoming requests
  virtual void on_event(uint8_t type, uint16_t value);
  
  // Connect to the two wire bus and service
  bool begin();
};

bool 
TWIslave::begin() 
{ 
  twi.set_buf(m_buf, sizeof(m_buf));
  return (twi.begin(this, ADDR)); 
}

void
TWIslave::on_event(uint8_t type, uint16_t value)
{
  for (uint8_t i = 1; i < sizeof(m_buf); i++)
    m_buf[i] = m_buf[0] + i;
}

// The TWI slave
TWIslave slave;

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaTWISlave: started"));

  // Check amount of free memory and size of classes
  TRACE(free_memory());
  TRACE(sizeof(TWIslave));
  TRACE(sizeof(OutputPin));

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
