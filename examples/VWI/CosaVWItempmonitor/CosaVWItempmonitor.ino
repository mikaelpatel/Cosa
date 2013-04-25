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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Demonstration receiving temperature readings from 1-Wire DS18B20
 * device over the Virtual Wire Interface (VWI). The measurements are
 * sent by the CosaVWItempsensor sketch running on an ATtiny85. 
 * VWI is used in extended mode with node addressing, message numbering,
 * and sub-net mask filtering.
 *
 * @section Circuit
 * Connect RF433/315 Receiver to Arduino D8.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/FixedPoint.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Virtual Wire Interface Receiver connected to pin D8
VirtualWireCodec codec;
VWI::Receiver rx(Board::D8, &codec);
const uint16_t SPEED = 4000;
const uint32_t ADDR = 0xc05a0000UL;
const uint32_t MASK = 0xffffff00UL;

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWItempmonitor: started"));
  TRACE(free_memory());

  // Start watchdog for low power delay
  Watchdog::begin();

  // Start virtual wire interface and receiver. Use eight bit sub-net mask
  // Transmitters must have the same 24 MSB address bits as the receiver.
  VWI::begin(ADDR, SPEED);
  rx.begin(MASK);
}

// Message from the sender
const uint8_t SAMPLE_CMD = 42;
struct sample_t {
  int16_t temperature[2];
  uint16_t voltage;
};

// Message received from VWI in extended mode
struct msg_t {
  VWI::header_t header;
  union {
    sample_t sample;
  };
};

IOStream& operator<<(IOStream& outs, FixedPoint& value)
{
  uint16_t fraction = value.get_fraction(2);
  outs << value.get_integer() << '.';
  if (fraction < 10) outs << '0';
  outs << fraction;
  return (outs);
}

void loop()
{
  // Receive a message. Sanity check the message size
  msg_t msg;
  int8_t len = rx.recv(&msg, sizeof(msg));
  if (len <= 0) return;

  // Print message header; transmitter address and sequence number
  trace << hex << msg.header.addr << ':' << msg.header.nr << ':';

  // Check message type and print contents
  if (msg.header.cmd == SAMPLE_CMD) {
    FixedPoint temp0(msg.sample.temperature[0], 4);
    FixedPoint temp1(msg.sample.temperature[1], 4);
    trace << PSTR("sample:") 
	  << temp0 << ':' << temp1 << ':' 
	  << msg.sample.voltage / 1000 << '.' << msg.sample.voltage % 1000 
	  << endl;
  }
  else {
    trace << msg.header.cmd << ':' << PSTR("unknown message type") << endl;
  }
}
