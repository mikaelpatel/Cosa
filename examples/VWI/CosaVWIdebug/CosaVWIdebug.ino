/**
 * @file CosaVWIdebug.ino
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
 * Handy tool for debugging message streams. The sketch will 
 * listen to transmissions and print received messages. Checks
 * if the message is printable otherwise printed as a hex dump.
 *
 * @section Circuit
 * Connect RF433/315 Receiver to Arduino D8.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Memory.h"

VirtualWireCodec codec;
VWI::Receiver rx(Board::D8, &codec);
const uint16_t SPEED = 4000;
const uint32_t TIMEOUT = 1000;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIdebug: started"));
  TRACE(free_memory());
  VWI::begin(SPEED);
  rx.begin();
}

void loop()
{
  static uint32_t start = 0L;
  static uint16_t nr = 0;

  char buffer[VWI::PAYLOAD_MAX];
  rx.await();
  int8_t len = rx.recv(buffer, sizeof(buffer), TIMEOUT);
  if (len <= 0) return;
  uint32_t stop = RTC::millis();

  // Check if the message is printable
  bool is_ascii = true;
  for (uint8_t i = 0; i < len; i++) {
    if ((buffer[i] < ' ' && buffer[i] != '\n') || buffer[i] > 127) {
      is_ascii = false;
      break;
    }
  }

  // Print prefix with delta time and length of message
  trace << nr++ << ':' << stop - start << ':' << len << ':';
  start = stop;
  if (is_ascii) {
    trace << '"';
    for (uint8_t i = 0; i < len; i++)
      if (buffer[i] != '\n') 
	trace << buffer[i];
      else 
	trace << PSTR("\\n");
    trace << '"' << endl;
  }
  else {
    trace.print(buffer, len, IOStream::hex, sizeof(buffer));
  }
}
