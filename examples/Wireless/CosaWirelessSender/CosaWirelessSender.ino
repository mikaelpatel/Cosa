/**
 * @file CosaWirelessSender.ino
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
 * Cosa Wireless interface demo; send messages; broadcast and
 * to nodes 1 to 3 (CosaWirelessReceiver).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Select Wireless device driver
// #include "Cosa/Wireless/Driver/CC1101.hh"
// CC1101 rf(0xC05A, 0x02);

#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(0xC05A, 0x02);

// #include "Cosa/Wireless/Driver/VWI.hh"
// #include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
// VirtualWireCodec codec;
// #if defined(__ARDUINO_TINYX5__)
// VWI rf(0xC05A, 0x03, 4000, Board::D1, Board::D0, &codec);
// #else
// VWI rf(0xC05A, 0x02, 4000, Board::D7, Board::D8, &codec);
// #endif

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessSender: started"));
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

// Message from the device; message string
static const uint8_t PAYLOAD_MAX = 14;
struct msg_t {
  uint8_t nr;
  uint8_t payload[PAYLOAD_MAX];
};
static const uint8_t PAYLOAD_TYPE = 0x00;

void loop()
{
  static msg_t msg = { 0 };
  
  // Send to nodes 0x01 to 0x03
  for (uint8_t dest = 0x01; dest < 0x04; dest++) {
    rf.send(dest, PAYLOAD_TYPE, &msg, sizeof(msg));
    msg.nr += 1;
  }

  // Update message; increment bytes
  for (uint8_t i = 0; i < PAYLOAD_MAX; i++) msg.payload[i] += 1;

  // Sleep in power down mode
  rf.powerdown();
  SLEEP(2);
}
