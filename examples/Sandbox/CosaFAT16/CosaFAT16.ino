/**
 * @file CosaFAT16.ino
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
 * @section Description
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI/Driver/SD.hh"
#include "Cosa/FS/FAT16.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"

SD sd;

void setup()
{
  uart.begin(9600);
  Watchdog::begin();
  RTC::begin();
  trace.begin(&uart, PSTR("CosaFAT16: started"));
  ASSERT(sd.begin(SPI::DIV4_CLOCK));
  ASSERT(FAT16::begin(&sd));
}

void loop()
{
  uint32_t start, stop;
  FAT16::File file;

  FAT16::ls(trace, FAT16::LS_DATE | FAT16::LS_SIZE);
  trace << endl;

  start = RTC::millis();
  ASSERT(file.open("TMP.TXT", O_WRITE | O_CREAT));
  for (uint8_t i = 0; i < 4; i++)
    file.write(NULL, 1024);
  ASSERT(file.close());
  stop = RTC::millis();
  trace << PSTR("Open/Write/Close file (4 KByte):");
  trace << stop - start << PSTR(" ms") << endl;
  trace << endl;

  FAT16::ls(trace, FAT16::LS_DATE | FAT16::LS_SIZE);
  trace << endl;

  start = RTC::millis();
  ASSERT(FAT16::rm("TMP.TXT"));
  stop = RTC::millis();
  trace << PSTR("Remove file:") << stop - start << PSTR(" ms") << endl;
  trace << endl;

  FAT16::ls(trace, FAT16::LS_DATE | FAT16::LS_SIZE);
  trace << endl;

  start = RTC::millis();
  ASSERT(file.open("NISSE.TXT", O_WRITE | O_TRUNC | O_CREAT));
  ASSERT(file.is_open());
  char msg[] = "Nisse badar.\n";
  ASSERT(file.write(msg, strlen(msg)) == (int) strlen(msg));
  ASSERT(file.close());
  stop = RTC::millis();
  trace << PSTR("Open/Write/Close file (13 Byte):");
  trace << stop - start << PSTR(" ms") << endl;

  start = RTC::millis();
  ASSERT(file.open("NISSE.TXT", O_READ));
  ASSERT(file.is_open());
  int c;
  while ((c = file.getchar()) != -1) trace << (char) c; 
  ASSERT(file.close());
  stop = RTC::millis();
  trace << PSTR("Open/Read/Close file (13 Byte):");
  trace << stop - start << PSTR(" ms") << endl;
  trace << endl;

  ASSERT(sd.end());
  ASSERT(true == false);
}
