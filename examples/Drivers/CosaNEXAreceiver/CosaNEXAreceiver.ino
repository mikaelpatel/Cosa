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
 * Simple sketch to demonstrate receiving Nexa Home Wireless Switch
 * Remote codes. First command received will be used as the device 
 * identity. Sucessive commands are compared against the device 
 * identity and if matches the built-in LED is set on/off according 
 * to the command. See Also CosaNEXAsender if you wish to run the 
 * sketch without a NEXA remote control.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Driver/NEXA.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

#if !defined(__ARDUINO_TINYX5__)
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
#endif

OutputPin led(Board::LED);
NEXA::Receiver receiver(Board::EXT0);
NEXA::code_t device = 0;

void setup()
{
#if !defined(__ARDUINO_TINYX5__)
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNEXAreceiver: started"));
  TRACE(free_memory());
#endif
  
  // Initiate Real-Time Clock and Watchdog
  RTC::begin();
  Watchdog::begin();

  // Use polling version to receive the remote button for device
  receiver.recv(device);
#if !defined(__ARDUINO_TINYX5__)
  trace << PSTR("learning: ") << device << endl;
#endif
  
  // Enable the interrupt driven version
  receiver.enable();
}

void loop()
{
  // Wait for the next event
  Event event;
  Event::queue.await(&event);
  uint8_t type = event.get_type();
  Event::Handler* handler = event.get_target();

  // Check that the event is an read completed and from the correct source
  if ((type != Event::READ_COMPLETED_TYPE) || (handler != &receiver)) return;
    
  // Get the received command code and check if it is for this device
  NEXA::code_t code = receiver.get_code();
  if (code == device) {
#if !defined(__ARDUINO_TINYX5__)
    trace << PSTR("matched: ") << code << endl;
#endif
    led << code.onoff;
  }
}
