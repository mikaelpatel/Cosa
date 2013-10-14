/**
 * @file CosaWirelessReceiver.ino
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
 * Cosa Wireless interface demo; receiver messages from CosaWirelessSender.
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
// VWI rf(0xC05A, 0x02, 4000, Board::D7, Board::D8, &codec);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessReceiver: started"));
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

void loop()
{
  // Receive message
  const uint8_t MSG_MAX = 16;
  uint8_t msg[MSG_MAX];
  uint8_t src;
  int count = rf.recv(src, msg, sizeof(msg), 1000L);

  // Check error codes
  if (count == -1) {
    trace << PSTR("error:illegal frame size(-1)\n");
  }
  else if (count == -2) {
    trace << PSTR("error:timeout(-2)\n");
  }
  else if (count < 0) {
    trace << PSTR("error(") << count << PSTR(")\n");
  }

  // Print the message
  else {
    trace << PSTR("msg(src = ") 
	  << hex << src 
	  << PSTR("):")
#if defined(__COSA_WIRELESS_DRIVER_CC1101_HH__)
	  << '[' << rf.get_input_power_level() << ',' 
	  << rf.get_link_quality_indicator() << PSTR("]:")
#endif
	  << count << ':';
    if (count > 0)
      trace.print(msg, count, IOStream::hex);
    else trace << msg << ':' << endl;
  }
}
