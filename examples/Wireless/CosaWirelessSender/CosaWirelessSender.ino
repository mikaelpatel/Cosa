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
#include "Cosa/Wireless/Driver/CC1101.hh"
CC1101 rf(0x02);

// #include "Cosa/Wireless/Driver/NRF24L01P.hh"
// NRF24L01P rf(0x02);

void setup()
{
  // uart.begin(9600);
  // trace.begin(&uart, PSTR("CosaWirelessSender: started"));
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

void loop()
{
  static const uint8_t MSG_MAX = 8;
  static uint8_t msg[MSG_MAX] = { 0x00 };
  static size_t len = 0;
  
  // Send message; broadcast(0x00) and send to nodes 0x01 to 0x03
  rf.broadcast(&msg, len = (len == 0) ? MSG_MAX : len - 1);
  for (uint8_t dest = 0x01; dest < 0x04; dest++)
    rf.send(dest, &msg, sizeof(msg));

  // Update message; increment bytes
  for (uint8_t i = 0; i < MSG_MAX; i++) {
    msg[i] += 1;
    if (msg[i] != 0) break;
  }

  // Sleep in power down mode
  rf.powerdown();
  SLEEP(2);
}
