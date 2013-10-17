/**
 * @file CosaWirelessDebug.ino
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
 * Demonstration of the Wireless Interface and device drivers.
 * Handy tool for debugging message streams. The sketch will 
 * listen to transmissions and print received messages. Checks
 * if the message is printable otherwise printed as a hex dump.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Select Wireless device driver
#include "Cosa/Wireless/Driver/CC1101.hh"
CC1101 rf(0xC05A, 0x01);

// #include "Cosa/Wireless/Driver/NRF24L01P.hh"
// NRF24L01P rf(0xC05A, 0x01);

// #include "Cosa/Wireless/Driver/VWI.hh"
// #include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
// nVirtualWireCodec codec;
// VWI rf(0xC05A, 0x01, 4000, Board::D7, Board::D8, &codec);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessDebug: started"));
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

void loop()
{
  // Receive message
  static uint32_t start = 0L;
  static uint16_t nr = 0;
  const uint8_t MSG_MAX = 64;
  uint8_t msg[MSG_MAX];
  uint8_t src;

  int count = rf.recv(src, msg, sizeof(msg));
  nr += 1;
  if (count <= 0) return;
  uint32_t stop = RTC::millis();

  // Check if the message is printable
  bool is_ascii = true;
  for (uint8_t i = 0; i < count; i++) {
    if ((msg[i] < ' ' && msg[i] != '\n') || msg[i] > 127) {
      is_ascii = false;
      break;
    }
  }

  // Print prefix with message number, delta time, source and length of message
  trace << nr << ':' 
	<< stop - start 
	<< PSTR(":src=") << src 
	<< PSTR(",dest=") << (rf.is_broadcast() ? 0 : rf.get_device_address())
	<< PSTR(",msg[") << count << PSTR("]=");
  if (is_ascii) {
    trace << '"';
    for (uint8_t i = 0; i < count; i++)
      if (msg[i] != '\n') 
	trace << (char) msg[i];
      else 
	trace << PSTR("\\n");
    trace << '"' << endl;
  }
  else 
    trace.print(msg, count, IOStream::hex, sizeof(msg));
  start = stop;
}
