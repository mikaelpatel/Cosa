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
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

#include <OWI.h>
#include <DS18B20.h>

// Configuration; network and device addresses
#define NETWORK 0xC05A
#define DEVICE 0x03
#define DEST 0x01

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

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessRelay: started"));
  Watchdog::begin();
  RTT::begin();
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
	  << PSTR(",dest=") << hex << rf.device_address()
	  << PSTR(",len=") << count
#if defined(COSA_WIRELESS_DRIVER_CC1101_HH)
	  << PSTR(",rssi=") << rf.input_power_level()
	  << PSTR(",lqi=") << rf.link_quality_indicator()
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
