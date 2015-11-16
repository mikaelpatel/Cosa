/**
 * @file CosaFAT16.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Short demo, test and benchmark of the FAT16/SD file access class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <SD.h>
#include <FAT16.h>

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

//#define USE_SD_ADAPTER
#define USE_SD_DATA_LOGGING_SHIELD
//#define USE_ETHERNET_SHIELD
//#define USE_TFT_ST7735

#if defined(WICKEDDEVICE_WILDFIRE) || defined(USE_SD_ADAPTER)
SD sd;

#elif defined(USE_ETHERNET_SHIELD)
SD sd(Board::D4);
OutputPin eth(Board::D10, 1);

#elif defined(USE_TFT_ST7735)
SD sd;
OutputPin tft(Board::D10, 1);

#elif defined(USE_SD_DATA_LOGGING_SHIELD)
SD sd(Board::D10);
#endif

#define SLOW_CLOCK SPI::DIV4_CLOCK
#define FAST_CLOCK SPI::DIV2_CLOCK
#define CLOCK FAST_CLOCK

void setup()
{
  Watchdog::begin();
  RTT::begin();
  uart.begin(115200);
  trace.begin(&uart, PSTR("CosaFAT16: started"));
  TRACE(free_memory());
  TRACE(sizeof(FAT16::File));
  ASSERT(sd.begin(CLOCK));
  ASSERT(FAT16::begin(&sd));
}

void loop()
{
  static const size_t BUF_MAX = 512;
  static const uint8_t K_BYTE = 100;
  uint8_t buf[BUF_MAX];
  uint32_t start, stop, ms;
  FAT16::File file;
  uint32_t size;
  int count;
  int c;

  // List the files
  ASSERT(!file.is_open());
  FAT16::ls(trace, FAT16::LS_DATE | FAT16::LS_SIZE);
  trace << endl;
  trace.flush();

  // Measure time to open/create a file. List the files
  start = RTT::millis();
  ASSERT(file.open("TMP.TXT", O_WRITE | O_CREAT));
  stop = RTT::millis();
  ms = stop - start;
  trace << PSTR("Open file:") << ms << PSTR(" ms") << endl;
  FAT16::ls(trace, FAT16::LS_DATE | FAT16::LS_SIZE);
  trace << endl;
  trace.flush();

  // Measure time to write a large block (1 K)
  start = RTT::millis();
  for (uint8_t i = 0; i < K_BYTE; i++) file.write(NULL, 1024);
  // IOStream cout(&file);
  // cout.print((void*) NULL, 1024, IOStream::hex);
  stop = RTT::millis();
  ms = stop - start;
  trace << PSTR("Write file (") << K_BYTE << PSTR(" KByte,1024 Byte):")
	<< ms << PSTR(" ms (")
	<< K_BYTE * 1024.0 / ms << PSTR(" KB/s)")
	<< endl
	<< endl;
  trace.flush();

  // Measure time to close a file
  start = RTT::millis();
  ASSERT(file.close());
  stop = RTT::millis();
  ms = stop - start;
  trace << PSTR("Close file:") << ms << PSTR(" ms") << endl;
  FAT16::ls(trace, FAT16::LS_DATE | FAT16::LS_SIZE);
  trace << endl;
  trace.flush();

  // Repen the time and measure the time to read character
  ASSERT(file.open("TMP.TXT", O_READ));
  start = RTT::millis();
  size = 0;
  while ((c = file.getchar()) != -1) size++;
  stop = RTT::millis();
  ms = stop - start;
  ASSERT(file.close());
  trace << PSTR("Read file (") << size / 1024
	<< PSTR(" KByte,1 Byte):") << ms << PSTR(" ms (")
	<< size * 1.0 / ms << PSTR(" KB/s)")
	<< endl;
  trace.flush();

  // Reopen the time and measure the time to read a smaller buffer
  ASSERT(file.open("TMP.TXT", O_READ));
  size = 0;
  start = RTT::millis();
  while ((count = file.read(buf, sizeof(buf))) > 0) size += count;
  stop = RTT::millis();
  ms = stop - start;
  ASSERT(file.close());
  trace << PSTR("Read file (") << size / 1024
	<< PSTR(" KByte,") << sizeof(buf) << PSTR(" Byte):")
	<< ms << PSTR(" ms (")
	<< size * 1.0 / ms << PSTR(" KB/s)")
	<< endl
	<< endl;
  trace.flush();

  // Remove the file and list
  start = RTT::millis();
  ASSERT(FAT16::rm("TMP.TXT"));
  stop = RTT::millis();
  ms = stop - start;
  trace << PSTR("Remove file:") << ms << PSTR(" ms") << endl;
  FAT16::ls(trace, FAT16::LS_DATE | FAT16::LS_SIZE);
  trace << endl;
  trace.flush();

  // Open a new file and write a short message. Measure total time
  start = RTT::millis();
  ASSERT(file.open("NISSE.TXT", O_WRITE | O_TRUNC | O_CREAT));
  ASSERT(file.is_open());
  char msg[] = "Nisse badar.\n";
  size = strlen(msg);
  ASSERT(file.write(msg, strlen(msg)) == (int) size);
  ASSERT(file.close());
  stop = RTT::millis();
  ms = stop - start;
  trace << PSTR("Open/Write/Close file:");
  trace << ms << PSTR(" ms") << endl;
  trace.flush();

  // Reopen and read the message. Measure total time
  start = RTT::millis();
  ASSERT(file.open("NISSE.TXT", O_READ));
  ASSERT(file.is_open());
  while ((c = file.getchar()) != -1) trace << (char) c;
  ASSERT(file.close());
  stop = RTT::millis();
  ms = stop - start;
  trace << PSTR("Open/Read/Close file:");
  trace << ms << PSTR(" ms") << endl;
  trace << endl;
  trace.flush();

  ASSERT(sd.end());
  ASSERT(true == false);
}

