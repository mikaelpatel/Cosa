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
 * Demonstration of the Virtual Wire Interface (VWI) driver;
 * Transceiver with acknowledgement and automatic retransmission.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino D9, RF433/315
 * Receiver to Arduino D8. Connect VCC and GND. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/DS1307.hh"
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
VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
// BitstuffingCodec codec;

// Virtual Wire Interface Transceiver
VWI::Transceiver trx(Board::D8, Board::D9, &codec);

// Network configuration
const uint32_t ADDR = 0xc05a0000UL;
const uint32_t MASK = 0xffffff00UL;
const uint16_t SPEED = 4000;

// Message types
const uint8_t SAMPLE_CMD = 1;
struct sample_t {
  uint16_t luminance;
  uint16_t temperature;
};

IOStream& operator<<(IOStream& outs, sample_t& sample)
{ 
  outs << PSTR("sample:") 
       << sample.luminance << PSTR(", ") 
       << sample.temperature;
  return (outs);
}

const uint8_t STAT_CMD = 2;
struct stat_t {
  uint16_t voltage;
  uint16_t sent;
  uint16_t resent;
  uint16_t received;
  uint16_t failed;
};

IOStream& operator<<(IOStream& outs, stat_t& stat)
{ 
  outs << PSTR("stat:") 
       << stat.voltage << PSTR(", ")
       << stat.sent << PSTR(", ")
       << stat.resent << PSTR(", ")
       << stat.received << PSTR(", ")
       << stat.failed << PSTR(" (")
       << (stat.resent * 100L) / stat.sent << PSTR("%)");
  return (outs);
}

// Message from the time keeper
const uint8_t TIMEKEEPER_CMD = 3;

// Extended mode message with header
struct msg_t {
  VWI::header_t header;
  union {
    sample_t sample;
    stat_t stat;
    DS1307::timekeeper_t now;
  };
};

IOStream& operator<<(IOStream& outs, msg_t& msg)
{ 
  outs << hex << msg.header.addr << ':' 
       << msg.header.nr << ':'
       << msg.header.cmd << ':';
  return (outs);
}

void setup()
{
  // Start trace on UART. Print available free memory.
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWIserver: started"));
  TRACE(free_memory());

  // Start watchdog for low power delay
  Watchdog::begin();
  RTC::begin();
  
  // Start virtual wire interface transceiver
  VWI::begin(ADDR, SPEED);
  trx.begin(MASK);
}

void loop()
{
  // Wait for a message. Sanity check the length
  msg_t msg;
  trx.rx.await();
  int8_t len = trx.recv(&msg, sizeof(msg));
  if (len <= 0) return;

  // Print header, type message type and contents
  trace << msg;
  switch (msg.header.cmd) {
  case SAMPLE_CMD: 
    trace << msg.sample << endl; 
    break;
  case STAT_CMD: 
    trace << msg.stat << endl; 
    break;
  case TIMEKEEPER_CMD: 
    msg.now.print();
    trace.println();
    break;
  default:
    trace << PSTR("unknown message type") << endl;
  }
}
