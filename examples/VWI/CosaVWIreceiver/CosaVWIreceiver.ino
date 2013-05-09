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
 * Receive and print a simple message with 2x16-bit analog data sample.
 *
 * @section Circuit
 * Connect RF433/315 Receiver to Arduino D8.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/VWI/Codec/ManchesterCodec.hh"
#include "Cosa/VWI/Codec/BitstuffingCodec.hh"
#include "Cosa/VWI/Codec/Block4B5BCodec.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIsender.ino
VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
// BitstuffingCodec codec;

// Virtual Wire Interface Receiver connected to pin D8
VWI::Receiver rx(Board::D8, &codec);
const uint16_t SPEED = 4000;

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIreceiver: started"));
  TRACE(free_memory());

  // Start watchdog for low power delay
  Watchdog::begin();

  // Start virtual wire interface and receiver
  VWI::begin(SPEED);
  rx.begin();
}

// Message type to receive with support output operator
struct msg_t {
  uint16_t nr;
  uint16_t luminance;
  uint16_t temperature;
};

IOStream& operator<<(IOStream& outs, msg_t& msg)
{ 
  outs << msg.nr << PSTR(": ");
  outs << msg.luminance << PSTR(", ");
  outs << msg.temperature;
  return (outs);
}

void loop()
{
  // Wait for a message and print contents
  msg_t msg;
  int8_t len = rx.recv(&msg, sizeof(msg));
  if (len != sizeof(msg)) return;
  trace << msg << endl;
}
