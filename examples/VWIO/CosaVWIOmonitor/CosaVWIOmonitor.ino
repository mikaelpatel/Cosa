/**
 * @file CosaVWIOmonitor.ino
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
 * Demonstration of the Virtual Wire Interface driver.
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
#include "Cosa/Memory.h"

VirtualWireCodec codec;
VWI::Receiver rx(Board::D8, &codec);
const uint16_t TIMEOUT = 10000;
const uint16_t SPEED = 4000;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIOmonitor: started"));
  VWI::begin(SPEED);
  rx.begin();
}

void loop()
{
  char buffer[VWI::PAYLOAD_MAX + 1];
  int8_t len = rx.recv(buffer, VWI::PAYLOAD_MAX, TIMEOUT);
  if (len <= 0) return;
  buffer[len] = 0;
  trace.print(buffer);
}
