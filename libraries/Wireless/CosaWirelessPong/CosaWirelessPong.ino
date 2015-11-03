/**
 * @file CosaWirelessPong.ino
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
 * Cosa Wireless interface demo; receive-send messages from and to
 * CosaWirelessPing. Check for correct port(PING_TYPE) and message
 * size. Increment ping number and send as reply. Note: asymmetric
 * naming, pong does not know the ping device address.
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
#define NETWORK 0xC05A
#define DEVICE 0x81

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
  trace.begin(&uart, PSTR("CosaWirelessPong: started"));
  Watchdog::begin();
  RTT::begin();
  ASSERT(rf.begin());
#if defined(USE_LOW_POWER)
  rf.output_power_level(-18);
#endif
}

void loop()
{
  uint8_t port;
  uint8_t src;
  ping_t nr;

  while (rf.recv(src, port, &nr, sizeof(nr)) != sizeof(nr)) yield();
  if (port != PING_TYPE) return;
  trace << RTT::millis() << PSTR(":pong:nr=") << nr << endl;
  nr += 1;
  rf.send(src, port, &nr, sizeof(nr));
}
