/**
 * @file CosaNEXAreceiver.ino
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
 * Simple sketch to capture Nexa Home Wireless Switch Remote codes.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Driver/NEXA.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

OutputPin led(Board::LED);
NEXA::Receiver receiver(Board::EXT0);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNEXAreceiver: started"));
  TRACE(free_memory());
  RTC::begin();
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  led.toggle();
  receiver.read_code().println(trace);
  led.toggle();
  receiver.enable();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  uint8_t type = event.get_type();
  Event::Handler* handler = event.get_target();
  if ((type == Event::READ_COMPLETED_TYPE) && (handler == &receiver)) {
    led.toggle();
    receiver.get_code().println(trace);
    led.toggle();
  }
}
