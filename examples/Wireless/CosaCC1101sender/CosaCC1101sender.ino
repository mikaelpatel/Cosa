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
 * Simple CC1101 transmitter demo; sends a message; broadcast and
 * to nodes 2 and 3.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Wireless/Driver/CC1101.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

CC1101 rf(0x01);

// #define DEBUG_CC1101
#if defined(DEBUG_CC1101)
void dump(uint8_t addr, uint8_t* reg, size_t size)
{
  for (uint8_t i = 0; i < size; i++) 
    trace << hex << i + addr << ':' << reg[i] 
	  << PSTR(" (") << hex << reg[i] << PSTR(")") << endl;
}
#endif

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCC1101sender: started"));
  Watchdog::begin();
  RTC::begin();

#if defined(DEBUG_CC1101)
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
  // Message; store source address as first byte
  static const uint8_t MSG_MAX = 8;
  static uint8_t msg[MSG_MAX] = { 0x01, 0x00, };

  // Send message; broadcast (2 bytes), node(0x02) and node(0x03)
  rf.send(0x00, &msg, 2);
  rf.send(0x02, &msg, sizeof(msg));
  rf.send(0x03, &msg, sizeof(msg));

  // Update message; increment bytes(1..7)
  for (uint8_t i = 1; i < MSG_MAX; i++) {
    msg[i] += 1;
    if (msg[i] != 0) break;
  }

  // Sleep in power down mode
  rf.set_power_down_mode();
  SLEEP(1);
}
