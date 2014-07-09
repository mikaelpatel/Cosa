/**
 * @file CosaS25FL127S.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * Demonstration the S25FL127S SPI Flash Memory device driver.
 * Measure performance of read and write.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI/Driver/S25FL127S.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/RTC.hh"

S25FL127S flash;

#if defined(ANARDUINO_MINIWIRELESS)
OutputPin rf_cs(Board::D10, 1);
#endif

void setup()
{
  Watchdog::begin();
  RTC::begin();
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaS25FL127S: started"));
  TRACE(free_memory());
  TRACE(sizeof(flash));

  // Initiate the flash memory device driver
  ASSERT(flash.begin());

  // Read configuration
  uint8_t config = flash.read_config();
  ASSERT(config == 0);
  trace << PSTR("config: ") << hex << config << endl;
  
  // Search for first non zero byte and clear next bit
  uint32_t addr = 0L;
  uint8_t data = 0;
  while (1) {
    ASSERT(flash.read(&data, addr, sizeof(data)) == sizeof(data));
    if (data != 0) break;
    addr += 1;
  }
  data >>= 1;
  uint32_t start = RTC::micros();
  int res = flash.write(addr, &data, sizeof(data));
  uint32_t us = RTC::micros() - start;
  ASSERT(flash.is_ready());
  ASSERT(res == sizeof(data));
  trace << PSTR("write: dest = ") << hex << addr 
	<< PSTR(", bytes = ") << sizeof(data)
	<< PSTR(", us = ") << us 
	<< PSTR(", Kbyte/s = ") << 1000.0 * sizeof(data) / us
	<< endl;
  SLEEP(5);

  // Write a buffer (512 byte) of test data across three pages
  addr = 128;
  uint8_t buf[512];
  memset(buf, 0xa5, sizeof(buf));
  start = RTC::micros();
  res = flash.write(addr, buf, sizeof(buf));
  us = RTC::micros() - start;
  ASSERT(res == sizeof(buf));
  trace << PSTR("write: dest = ") << hex << addr 
	<< PSTR(", bytes = ") << sizeof(buf)
	<< PSTR(", us = ") << us 
	<< PSTR(", Kbyte/s = ") << 1000.0 * sizeof(buf) / us
	<< endl;
  SLEEP(5);

  // Locate end of file in first sector; binary search
  start = RTC::micros();
  uint16_t bit = 0x8000;
  uint32_t last = 0L;
  do {
    addr = last | bit;
    res = flash.read(&data, addr, sizeof(data));
    if (res != sizeof(data)) break;
    if (data != 0xff) last |= bit;
    bit >>= 1;
  } while (bit != 0);
  us = RTC::micros() - start;
  ASSERT(res == sizeof(data));
  ASSERT(flash.read(&data, last, sizeof(data)) == sizeof(data));
  trace << PSTR("bsearch: last = ") << hex << last
	<< PSTR(", data = ") << hex << data
	<< PSTR(", ms = ") << us / 1000.0 
	<< endl;
  ASSERT(flash.read(&data, last + 1, sizeof(data)) == sizeof(data));
  ASSERT(data == 0xff);
  SLEEP(5);

  // Locate end of file in first sector; reverse search
  start = RTC::micros();
  last = 0x10000L;
  for (uint8_t i = 0; i < 128; i++) {
    last -= sizeof(buf);
    res = flash.read(&buf, last, sizeof(buf));
    if (res != sizeof(buf)) break;
    uint16_t j = sizeof(buf) - 1;
    while (buf[j] == 0xff) if (j == 0) break; else j--;
    if (j == 0) continue;
    last += j;
    break;
  }
  us = RTC::micros() - start;
  ASSERT(res == sizeof(buf));
  ASSERT(flash.read(&data, last, sizeof(data)) == sizeof(data));
  trace << PSTR("rsearch: last = ") << hex << last
	<< PSTR(", data = ") << hex << data
	<< PSTR(", ms = ") << us / 1000.0
	<< endl;
  ASSERT(flash.read(&data, last + 1, sizeof(data)) == sizeof(data));
  ASSERT(data == 0xff);
  SLEEP(5);
}

void loop()
{
  // Read status and configuration registers
  trace << PSTR("status: ") << hex << flash.read_status1() 
	<< PSTR(", ") << hex << flash.read_status2()
	<< endl;

  // Read a block from the flash; measure time
  uint8_t buf[512];
  static uint32_t src = 0L;
  uint32_t start = RTC::micros();
  int res = flash.read(buf, src, sizeof(buf));
  uint32_t us = RTC::micros() - start;
  ASSERT(res == sizeof(buf));
  trace << PSTR("read: src = ") << hex << src
	<< PSTR(", bytes = ") << sizeof(buf) 
	<< PSTR(", us = ") << us
	<< PSTR(", Kbyte/s = ") << 1000.0 * sizeof(buf) / us
	<< endl;
  trace.print(buf, sizeof(buf), IOStream::hex);
  src += sizeof(buf);

  // Step through the flash memory
  SLEEP(5);
}
