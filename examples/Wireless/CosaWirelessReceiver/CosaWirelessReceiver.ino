/**
 * @file CosaCC1101receiver.ino
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
 * Simple CC1101 receiver demo; receive messages
 *
 * Implementation work on the CC1101 device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Wireless/Driver/CC1101.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

CC1101 rf(0x02);

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
  trace.begin(&uart, PSTR("CosaCC1101receiver: started"));
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
  // Receive message
  const uint8_t MSG_MAX = 32;
  uint8_t msg[MSG_MAX];
  uint8_t src;
  int count = rf.recv(src, msg, sizeof(msg), 1000L);
  // Check error codes
  if (count == -1) {
    trace << PSTR("illegal frame size(-1)\n");
  }
  else if (count == -2) {
    trace << PSTR("timeout(-2)\n");
  }
  else if (count < 0) {
    trace << PSTR("error(") << count << PSTR(")\n");
  }
  // Print the message
  else {
    trace << hex << src << PSTR(":[") 
	  << rf.get_input_power_level() << ',' 
	  << rf.get_link_quality_indicator() << PSTR("]:")
	  << count << ':';
    if (count > 0)
      trace.print(msg, count, IOStream::hex);
    else trace << msg << ':' << endl;
  }
}
