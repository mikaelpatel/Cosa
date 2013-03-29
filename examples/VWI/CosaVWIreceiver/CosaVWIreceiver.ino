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
 * and 2x16-bit analog data sample.
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

// Virtual Wire Interface Receiver connected to pin D9
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

// Message type to receive
struct msg_t {
  uint32_t id;
  uint8_t nr;
  uint16_t data[2];
};

void loop()
{
  // Check message number and count errors
  static uint8_t next = 0;
  static uint32_t err = 0;
  static uint32_t cnt = 0;

  // Wait for a message
  rx.await();
  msg_t msg;
  int8_t len = rx.recv(&msg, sizeof(msg));

  // Check that the correct messaage size was received
  if (len != sizeof(msg)) return;
  cnt += 1;

  // Check message number 
  if (msg.nr != next) {
    for (uint8_t i = next; i < msg.nr; i++) {
      trace << i << ':' << PSTR("missing") << endl;
      err += 1;
    }
    next = msg.nr;
    trace << (100 * err) / cnt << PSTR(" % drop rate") << endl;
  }

  // Print message contents
  trace << hex << msg.id << ':' << msg.nr << ':';
  trace << hex << msg.data[0] << PSTR(", ") << hex << msg.data[1] << endl;
  next += 1;
}
