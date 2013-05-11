/**
 * @file CosaVWImonitor.ino
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
 * Print incoming messages as a character stream.
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

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWImonitor: started"));
  TRACE(free_memory());
  RTC::begin();
  VWI::begin(4000);
  rx.begin();
}

void loop()
{
  rx.await();
  char buffer[VWI::PAYLOAD_MAX];
  int8_t len = rx.recv(buffer, sizeof(buffer));
  uint8_t is_ascii = 1;
  static uint32_t start = 0L;
  static uint16_t nr = 0;
  for (uint8_t i = 0; i < len; i++) {
    if (buffer[i] < ' ' || buffer[i] > 127) {
      is_ascii = 0;
      break;
    }
  }
  if (is_ascii) {
    for (uint8_t i = 0; i < len; i++)
      trace.print(buffer[i]);
  }
  else {
    uint32_t stop = RTC::millis();
    trace.print(nr++);
    trace.print(':');
    trace.print(stop - start);
    trace.print(':');
    trace.print(buffer, len, IOStream::hex, sizeof(buffer));
    start = stop;
  }
}
