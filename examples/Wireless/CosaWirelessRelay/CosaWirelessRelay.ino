/**
 * @file CosaWirelessRelay.ino
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
 * Cosa Wireless interface demo; relay messages from CosaWirelessSender
 * and forward to CosaWirelessReceiver.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Configuration; network and device addresses
#define NETWORK 0xC05A
#define DEVICE 0x03
#define DEST 0x01

// Select Wireless device driver
// #define USE_CC1101
#define USE_NRF24L01P
// #define USE_RFM69
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
#if defined(BAORD_TINY)
VWI rf(NETWORK, DEVICE, BPS, Board::D1, Board::D0, &codec);
#else
VWI rf(NETWORK, DEVICE, BPS, Board::D7, Board::D8, &codec);
#endif
#endif

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessRelay: started"));
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

void loop()
{
  // Receive a message
  const uint32_t TIMEOUT = 5000;
  const uint8_t MSG_MAX = rf.PAYLOAD_MAX;
  uint8_t msg[MSG_MAX];
  uint8_t src;
  uint8_t port;
  int count = rf.recv(src, port, msg, sizeof(msg), TIMEOUT);

  // Print the message header
  if (count >= 0 && !rf.is_broadcast()) {
    trace << PSTR("src=") << hex << src
	  << PSTR(",port=") << hex << port
	  << PSTR(",dest=") << hex << rf.get_device_address()
	  << PSTR(",len=") << count
#if defined(COSA_WIRELESS_DRIVER_CC1101_HH)
	  << PSTR(",rssi=") << rf.get_input_power_level()
	  << PSTR(",lqi=") << rf.get_link_quality_indicator()
#endif
	  << endl;
    rf.send(DEST, port, msg, count);
  }

  // Check error codes
  else if (count == -1) {
    trace << PSTR("error:illegal frame size(-1)") << endl;
  }
  else if (count == -2) {
    trace << PSTR("error:timeout(-2)") << endl;
  }
  else if (count < 0) {
    trace << PSTR("error(") << count << PSTR(")") << endl;
  }
}
