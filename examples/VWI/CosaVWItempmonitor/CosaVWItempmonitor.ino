/**
 * @file CosaVWItempmonitor.ino
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
 * @section Description
 * Demonstration receiving temperature readings from a OneWire DS18B20
 * device over the Virtual Wire Interface (VWI). The measurements are
 * sent by the CosaVWItempsensor sketch running on an ATtiny85.
 *
 * @section Circuit
 * Connect RF433/315 Receiver to Arduino D11.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OWI.hh"
#include "Cosa/FixedPoint.hh"
#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Virtual Wire Interface Receiver connected to pin D11
VirtualWireCodec codec;
VWI::Receiver rx(Board::D11, &codec);
const uint16_t SPEED = 4000;

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWItempmonitor: started"));
  TRACE(free_memory());

  // Start watchdog for low power delay
  Watchdog::begin();

  // Start virtual wire interface and receiver
  VWI::begin(SPEED);
  rx.begin();
}

struct msg_t {
  uint8_t id[OWI::ROM_MAX];
  uint16_t nr;
  int16_t temperature;
};

void loop()
{
  // Check message number and count errors
  static uint16_t next = 0;
  static uint32_t err = 0;
  static uint32_t cnt = 0;

  // Wait for a message
  rx.await();
  msg_t msg;
  int8_t len = rx.recv(&msg, sizeof(msg));

  // Check that the correct messaage size was received
  if (len != sizeof(msg)) return;

  // Check message number 
  if (msg.nr != next) {
    next = msg.nr;
    err += 1;
  }
  cnt += 1;

  // Print message contents; 1-wire identity in hex
  for (uint8_t i = 0; i < membersof(msg.id); i++) {
    uint8_t high = msg.id[i] >> 4;
    uint8_t low = msg.id[i] & 0xf;
    trace << (char) (high + (high > 9 ? 'a' - 10 : '0'));
    trace << (char) (low + (low > 9 ? 'a' - 10: '0'));
  }

  // Followed by message sequence number
  trace << ':' << msg.nr << ':';

  // Followed by the temperature reading
  FixedPoint temp(msg.temperature, 4);
  int16_t integer = temp.get_integer();
  uint16_t fraction = temp.get_fraction(2);
  trace << integer << '.';
  if (fraction < 10) trace << '0';
  trace << fraction << endl;
  next += 1;

  // Print message count and errors every 256 messages
  if (next & 0xff == 0) {
    trace << PSTR("count = ") << cnt << endl;
    trace << PSTR("errors = ") << err << endl;
  }
}
