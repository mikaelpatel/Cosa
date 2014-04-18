/**
 * @file CosaDS1302.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Demonstration of Cosa DS1302 Trickle-Charge Timekeeping Chip,
 * device driver.
 * 
 * @section Circuit
 *                         DS1302/rtc 
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (D2)----------------3-|CLK         |
 * (D3)----------------4-|DAT         |
 * (D4)----------------5-|RST         |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Driver/DS1302.hh"

DS1302 rtc;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS1302: started"));
  Watchdog::begin();
  
  // Read device static memory
  trace << PSTR("RAM");
  for (uint8_t addr = 0; addr < sizeof(uint32_t); addr++)
    trace  << ':' << hex << rtc.read(DS1302::RAM_START + addr);
  trace << endl;

  // Write some data
  uint32_t key = 0x12345678UL;
  rtc.write_protect(false);
  rtc.write_ram(&key, sizeof(key));
  rtc.write_protect(true);

  // Burst read and check
  rtc.read_ram(&key, sizeof(key));
  trace << PSTR("RAM:") << hex << key << endl;
  ASSERT(key == 0x12345678UL);

  // Set the clock and calender
#if defined(SET_RTC)
  time_t now;
  now.seconds = 0x00;
  now.minutes = 0x00;
  now.hours = 0x21;
  now.date = 0x18;
  now.month = 0x04;
  now.day = 0x05;
  now.year = 0x14;
  rtc.set_time(now);
#endif
}

void loop()
{
  // Read clock and calender; low and high level
  trace << PSTR("RTC");
  for (uint8_t addr = 0; addr < 9; addr++) 
    trace << ':' << hex << rtc.read(addr);
  trace << endl;
  time_t now;
  rtc.get_time(now);
  trace << now << endl;

  // Take a nap
  SLEEP(5);
}
