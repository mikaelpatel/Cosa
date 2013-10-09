/**
 * @file CosaCC1101sender.ino
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
 * Simple CC1101 transmitter demo; send messages; broadcast and
 * to nodes 1 to 3.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Wireless/Driver/CC1101.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

CC1101 rf(0x01);

// #define DEBUG_CC1101
#if defined(DEBUG_CC1101)
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void dump(uint8_t addr, uint8_t* reg, size_t size)
{
  for (uint8_t i = 0; i < size; i++) 
    trace << hex << i + addr << ':' << reg[i] 
	  << PSTR(" (") << hex << reg[i] << PSTR(")") << endl;
}
#endif

void setup()
{
  Watchdog::begin();
  RTC::begin();

#if defined(DEBUG_CC1101)
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCC1101sender: started"));
  uint8_t reg[CC1101::CONFIG_MAX];
  INFO("Read default register values", 0);
  rf.read(CC1101::IOCFG2, reg, sizeof(reg));
  dump(CC1101::IOCFG2, reg, sizeof(reg));
  INFO("Start device and read registers", 0);
#endif

  rf.begin();

#if defined(DEBUG_CC1101)
  rf.read(CC1101::IOCFG2, reg, sizeof(reg));
  dump(CC1101::IOCFG2, reg, sizeof(reg));
  INFO("Read status registers ", 0);
  for (uint8_t i = 0; i < CC1101::STATUS_MAX; i++) 
    reg[i] = rf.read((CC1101::Status) (CC1101::PARTNUM + i));
  dump(CC1101::PARTNUM, reg, CC1101::STATUS_MAX);
#endif
}

void loop()
{
  static const uint8_t MSG_MAX = 16;
  static uint8_t msg[MSG_MAX] = { 0x00 };
  static size_t len = MSG_MAX;
  
  // Send message; broadcast(0x00) and send to nodes 0x01 to 0x03
  rf.broadcast(&msg, len);
  for (uint8_t dest = 0x01; dest < 0x04; dest++)
    rf.send(dest, &msg, sizeof(msg));

  // Update message; increment bytes
  for (uint8_t i = 0; i < MSG_MAX; i++) {
    msg[i] += 1;
    if (msg[i] != 0) break;
  }
  len = (len == 0) ? MSG_MAX : len - 1;

  // Sleep in power down mode
  rf.powerdown();
  SLEEP(2);
}
