/**
 * @file CosaVWIreceiver.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver.
 * Receive and print a simple message with identity, message number,
 * and 32-bit data element.
 *
 * @section Circuit
 * Connect RF433/315 Receiver to Arduino D11.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Virtual Wire Interface Receiver connected to pin D11
VWI::Receiver rx(Board::D11);

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIreceiver: started"));
  TRACE(free_memory());

  // Start watchdog for low power delay
  Watchdog::begin();

  // Start virtual wire interface and receiver
  VWI::begin(2000);
  rx.begin();
}

// Message type to receive
struct msg_t {
  uint32_t id;
  uint8_t nr;
  uint32_t data;
};

void loop()
{
  // Check message number and count errors
  static uint8_t next = 0;
  static uint32_t err = 0;
  static uint32_t cnt = 0;

  char buf[VWI::MESSAGE_MAX];
  int8_t buflen = VWI::MESSAGE_MAX;

  // Receive message and print contents
  rx.await();
  buflen = rx.recv(buf, buflen);
  if (buflen > 0) {
    cnt += 1;

    // Check that the correct messaage size was received
    if (buflen != sizeof(msg_t)) {
      trace.print(buf, buflen, 16);
      err += 1;
    }
    else {
      // Check message number
      msg_t* msg = (msg_t*) buf;
      if (msg->nr != next) {
	next = msg->nr;
	err += 1;
      }
      // Print message contents
      trace << hex << msg->id << ':' << msg->nr << ':';
      trace << hex << msg->data << endl;
      next += 1;
    }
    // Print message count and errors every 256 messages
    if (next == 0) {
      trace << PSTR("count = ") << cnt << endl;
      trace << PSTR("errors = ") << err << endl;
    }
  }
}
