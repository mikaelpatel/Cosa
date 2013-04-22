/**
 * @file CosaVWIserver.ino
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
 * and 2x16-bit analog data sample. Send an acknowledge by sending 
 * a message with the received identity and message number.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino D9, RF433/315
 * Receiver to Arduino D8. Connect VCC and GND. 
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
#include "Cosa/RTC.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIclient.ino
// VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
BitstuffingCodec codec;

// Virtual Wire Interface Transmitter and Receiver
VWI::Transmitter tx(Board::D9, &codec);
VWI::Receiver rx(Board::D8, &codec);
const uint32_t ADDR = 0xC05A0000;
const uint16_t SPEED = 4000;
const uint8_t MASK = 8;

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIserver: started"));
  TRACE(free_memory());

  // Start watchdog for low power delay
  Watchdog::begin();
  RTC::begin();
  
  // Start virtual wire interface and receiver
  VWI::begin(ADDR, SPEED);
  rx.begin(MASK);
  tx.begin();
}

// Message to receiver from CosaVWIclient
const uint8_t SAMPLE_CMD = 17;
struct sample_t {
  uint16_t luminance;
  uint16_t temperature;
};

// Extended mode message with header
struct msg_t {
  VWI::header_t header;
  union {
    sample_t sample;
  };
};

void loop()
{
  static uint8_t nr = 0xff;

  // Wait for a message. Sanity check the length and message type/cmd
  msg_t msg;
  rx.await();
  int8_t len = rx.recv(&msg, sizeof(msg));
  if (len != sizeof(msg)) return;
  if (msg.header.cmd != SAMPLE_CMD) return;
  
  // Send an acknowledgement; echo the message header (using basic mode)
  iovec_t vec[2];
  vec[0].buf = &msg.header;
  vec[0].size = sizeof(msg.header);
  vec[1].buf = 0;
  vec[1].size = 0;
  tx.send(vec);
  tx.await();

  // Print message contents (only once)
  if (nr != msg.header.nr) {
    nr = msg.header.nr;
    trace << hex << msg.header.addr << ':' 
	  << msg.header.nr << ':' 
	  << msg.header.cmd << ':'
	  << hex << msg.sample.luminance << PSTR(", ")
	  << hex << msg.sample.temperature << endl;
  }
}
