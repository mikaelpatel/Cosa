/**
 * @file CosaWirelessPing.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Cosa Wireless interface demo; send-receive messages to and from
 * CosaWirelessPong. Send sequence number to pong and wait for reply
 * with incremented sequence number. Retransit after reply wait and
 * inter-message delay.
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

// Configuration; network and device addresses.
#define PING_ID 0x80
#define PONG_ID 0x81
#define NETWORK 0xC05A
#define DEVICE PING_ID

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
VWI::Transmitter tx(Board::D0, &codec);
VWI::Receiver rx(Board::D1, &codec);
#else
VWI::Transmitter tx(Board::D6, &codec);
VWI::Receiver rx(Board::D7, &codec);
#endif
VWI rf(NETWORK, DEVICE, SPEED, &rx, &tx);

typedef int16_t ping_t;
static const uint8_t PING_TYPE = 0x80;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessPing: started"));
  Watchdog::begin();
  RTT::begin();
  ASSERT(rf.begin());
#if defined(USE_LOW_POWER)
  rf.output_power_level(-18);
#endif
  trace << PSTR("nr=sequence number") << endl;
  trace << PSTR("rc=retransmission count") << endl;
  trace << PSTR("arc=accumulated retransmission count") << endl;
  trace << PSTR("dr%=drop rate procent (arc*100/(arc + nr))") << endl;
  trace << endl;
}

void loop()
{
  // Auto retransmission wait (ms) and acculated retransmission counter
  static const uint16_t ARW = 200;
  static uint16_t arc = 0;

  // Sequence number
  static ping_t nr = 0;

  // Receive port and source address
  uint8_t port;
  uint8_t src;

  // Send sequence number and receive update. Count number of retransmissions
  uint32_t now = RTT::millis();
  uint8_t rc = 0;
  trace << now << PSTR(":ping:nr=") << nr;
  while (1) {
    rf.send(PONG_ID, PING_TYPE, &nr, sizeof(nr));
    int res = rf.recv(src, port, &nr, sizeof(nr), ARW);
    if (res == (int) sizeof(nr)) break;
    rc += 1;
  }
  arc += rc;
  trace << PSTR(",pong:nr=") << nr
	<< PSTR(",rc=") << rc
	<< PSTR(",arc=") << arc
	<< PSTR(",dr%=") << (arc * 100L) / (nr + arc)
	<< endl;
  rf.powerdown();
  static const uint32_t PERIOD = 2000L;
  uint32_t ms = PERIOD - RTT::since(now);
  if (ms > PERIOD) ms = PERIOD;
  delay(ms);
}
