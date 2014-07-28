/**
 * @file CosaWirelessSender.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

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
// #define USE_CC1101
// #define USE_NRF24L01P
#define USE_RFM69
// #define USE_VWI

#if defined(USE_CC1101)
#include "Cosa/Wireless/Driver/CC1101.hh"
CC1101 rf(NETWORK, DEVICE);

#elif defined(USE_NRF24L01P)
#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(NETWORK, DEVICE);

#elif defined(USE_RFM69)
#include "Cosa/Wireless/Driver/RFM69.hh"
RFM69 rf(NETWORK, DEVICE);

#elif defined(USE_VWI)
#include "Cosa/Wireless/Driver/VWI.hh"
#include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
VirtualWireCodec codec;
#define BPS 4000
#if defined(BOARD_ATTINY)
VWI rf(NETWORK, DEVICE, BPS, Board::D1, Board::D0, &codec);
#else
VWI rf(NETWORK, DEVICE, BPS, Board::D7, Board::D8, &codec);
#endif
#endif

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessSender: started"));
  Watchdog::begin();
  RTC::begin();
  ASSERT(rf.begin());
  rf.set_output_power_level(-18);
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
    trace << PSTR("dest=") << dest << PSTR(",nr=") << msg.nr;
    res = rf.send(dest, PAYLOAD_TYPE, &msg, sizeof(msg));
    trace << PSTR(",res=") << res;
    if (res != sizeof(msg)) {
      trace << PSTR(":failed to send");
    }
    trace << endl;
    msg.nr += 1;
  }

  // Broadcast message
  trace << PSTR("dest=0,nr=") << msg.nr;
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
