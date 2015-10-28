/**
 * @file CosaWirelessTrace.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Demonstration of the Wireless Interface and device drivers. Handy
 * tool for tracing message streams. The sketch will listen to
 * transmissions and print received messages. Checks if the message is
 * printable otherwise printed as a hex dump. The sketch will receive
 * any broadcast or addressed message to the node(0x01) on all ports.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

// Configuration; network and device addresses
#define NETWORK 0xC05A
#define DEVICE 0x01

// Select Wireless device driver
// #include <CC1101.h>
// CC1101 rf(NETWORK, DEVICE);

// #include <NRF24L01P.h>
// NRF24L01P rf(NETWORK, DEVICE);

// #include <RFM69.h>
// RFM69 rf(NETWORK, DEVICE);

#include <VWI.h>
// #include <BitstuffingCodec.h>
// BitstuffingCodec codec;
// #include <Block4B5BCodec.h>
// Block4B5BCodec codec;
// #include <HammingCodec_7_4.h>
// HammingCodec_7_4 codec;
// #include <HammingCodec_8_4.h>
// HammingCodec_8_4 codec;
// #include <ManchesterCodec.h>
// ManchesterCodec codec;
#include <VirtualWireCodec.h>
VirtualWireCodec codec;
#define SPEED 4000
#if defined(BOARD_ATTINY)
VWI::Receiver rx(Board::D1, &codec);
#else
VWI::Receiver rx(Board::D7, &codec);
#endif
VWI rf(NETWORK, DEVICE, SPEED, &rx);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessTrace: started"));
  Watchdog::begin();
  RTT::begin();
  rf.begin();
}

void loop()
{
  const uint32_t TIMEOUT = 10000L;
  const uint8_t MSG_MAX = 64;
  static uint32_t start = 0L;
  static uint16_t nr = 0;
  uint8_t msg[MSG_MAX];
  uint8_t src;
  uint8_t port;

  // Receive message
  int count = rf.recv(src, port, msg, sizeof(msg), TIMEOUT);
  uint32_t stop = RTT::millis();

  // Print prefix with message number, delta time
  trace << nr << ':' << stop - start;
  nr += 1;

  // Check for errors
  if (count < 0) {
    trace << PSTR(":error(") << count << PSTR(")\n");
    return;
  }

  // Check if the message is printable
  bool is_ascii = true;
  for (uint8_t i = 0; i < count; i++) {
    if ((msg[i] < ' ' && msg[i] != '\n' && msg[i] != '\f') || msg[i] > 127) {
      is_ascii = false;
      break;
    }
  }

  // Print source, destination, port, and length of message
  trace << PSTR(":src=") << src
	<< PSTR(",dest=") << (rf.is_broadcast() ? 0 : rf.device_address())
	<< PSTR(",port=") << port
	<< PSTR(",msg[") << count << PSTR("]=");

  // Check if the message is printable as a string
  if (is_ascii) {
    trace << '"';
    for (uint8_t i = 0; i < count; i++)
      if (msg[i] == '\f')
	trace << PSTR("\\f");
      else if (msg[i] == '\n')
	trace << PSTR("\\n");
      else
	trace << (char) msg[i];
    trace << '"' << endl;
  }

  // Otherwise do a hex dump
  else
    trace.print(msg, count, IOStream::hex, sizeof(msg));

  // Save the timestamp
  start = stop;
}
