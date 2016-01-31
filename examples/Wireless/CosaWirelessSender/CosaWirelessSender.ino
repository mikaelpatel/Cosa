/**
 * @file CosaWirelessSender.ino
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
 * Cosa Wireless interface demo; send messages; broadcast and
 * to nodes 1 to 3 (CosaWirelessReceiver).
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

// Configuration; network and device addresses. Allow multiple senders
#define NETWORK 0xC05A
#if defined(BOARD_ATTINY)
#define DEVICE 0x10
#elif defined(BOARD_ATMEGA2560)
#define DEVICE 0x11
#else
#define DEVICE 0x12
#endif

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
#else
VWI::Transmitter tx(Board::D6, &codec);
#endif
VWI rf(NETWORK, DEVICE, SPEED, &tx);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessSender: started"));
  Watchdog::begin();
  RTT::begin();
  ASSERT(rf.begin());
  rf.output_power_level(-18);
}

// Message from the device; message string
static const uint8_t PAYLOAD_MAX = 16;
struct msg_t {
  uint8_t nr;
  uint8_t payload[PAYLOAD_MAX];
};
static const uint8_t PAYLOAD_TYPE = 0x01;

void loop()
{
  static msg_t msg = { 0 , { 0 } };
  int res;

  // Send to nodes 0x01 to 0x03
  for (uint8_t dest = 0x01; dest < 0x04; dest++) {
    trace << RTT::micros() << PSTR(":dest=") << dest << PSTR(",nr=") << msg.nr;
    res = rf.send(dest, PAYLOAD_TYPE, &msg, sizeof(msg));
    trace << PSTR(",res=") << res;
    if (res != sizeof(msg)) {
      trace << PSTR(":failed to send");
    }
    trace << endl;
    msg.nr += 1;
  }

  // Broadcast message
  trace << RTT::micros() << PSTR(":dest=0,nr=") << msg.nr;
  res = rf.broadcast(PAYLOAD_TYPE, &msg, sizeof(msg));
  trace << PSTR(",res=") << res;
  if (res != sizeof(msg)) {
    trace << PSTR(":failed to send");
  }
  trace << endl << endl;
  msg.nr += 1;

  // Update message; increment bytes
  for (uint8_t i = 0; i < PAYLOAD_MAX; i++) msg.payload[i] += 1;

  // Sleep in power down mode
  rf.powerdown();
  sleep(2);
}
