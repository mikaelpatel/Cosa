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
  trace << PSTR("write: dest = ") << addr 
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
  trace << PSTR("write: dest = ") << addr 
	<< PSTR(", bytes = ") << sizeof(buf)
	<< PSTR(", us = ") << us 
	<< PSTR(", Kbyte/s = ") << 1000.0 * sizeof(buf) / us
	<< endl;
  ASSERT(res == sizeof(buf));
  SLEEP(5);
}

void loop()
{
  // Read status and configuration registers
  trace << PSTR("status: ") << hex << flash.read_status1() 
	<< PSTR(", ") << hex << flash.read_status2()
	<< endl;

  // Read a block from the flash; measure time
  uint8_t buf[256];
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
