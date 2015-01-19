/**
 * @file CosaCFFS.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Demonstration the SPI Flash Memory device drivers and file
 * system.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/FS/CFFS.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/RTC.hh"

#define USE_FLASH_S25FL127S
//#define USE_FLASH_W25X40CL

#if defined(USE_FLASH_S25FL127S)
#include "Cosa/Flash/Driver/S25FL127S.hh"
S25FL127S flash;
#endif

#if defined(USE_FLASH_W25X40CL)
#include "Cosa/Flash/Driver/W25X40CL.hh"
W25X40CL flash;
#endif

void setup()
{
  // Initiate components and print memory usage
  Watchdog::begin();
  RTC::begin();
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCFFS: started"));
  TRACE(free_memory());
  TRACE(sizeof(flash));
  TRACE(sizeof(CFFS::File));

  MEASURE("Initiate the flash memory device driver:", 1)
    ASSERT(flash.begin());

  MEASURE("Format file system driver:", 1) {
    ASSERT(CFFS::format(&flash, "flash") == 0);
  }

  MEASURE("Initiate the file system driver:", 1) {
    ASSERT(CFFS::begin(&flash));
  }

  uint8_t buf[128];
  MEASURE("Reading file system and descriptors:", 1) {
    int res = flash.read(buf, 0L, sizeof(buf));
    ASSERT(res == sizeof(buf));
  }
  trace.print(buf, sizeof(buf), IOStream::hex);

  INFO("List files", 0);
  CFFS::ls(trace);

  INFO("Open and dump file if available", 0);
  CFFS::File file;
  if (file.open("Kalle", O_READ) == 0) {
    uint32_t size = file.size();
    for (uint32_t i = 0; i < size; i++)
      trace << (char) file.getchar();
    TRACE(file.close());
    MEASURE("Open file:", 1) ASSERT(file.open("Kalle", O_WRITE) == 0);
    TRACE(file.tell());
  }
  else {
    MEASURE("Open file:", 1) ASSERT(file.open("Kalle", O_CREAT | O_EXCL) == 0);
    CFFS::ls(trace);
    TRACE(file.tell());
  }
  MEASURE("Write log entry:", 1) {
    IOStream cout(&file);
    cout << RTC::micros() << PSTR(":A0 = ") 
	 << AnalogPin::sample(Board::A0)
	 << endl;
  }
  MEASURE("Tell position:", 1) file.tell();
  MEASURE("File size:", 1) file.size();
  MEASURE("Close file:", 1) file.close();
  MEASURE("Open log file:", 1) ASSERT(file.open("Kalle", O_READ) == 0);
  MEASURE("Read buffer:", 1) file.read(buf, sizeof(buf));

  INFO("Rewind and dump as text to output stream", 0);
  TRACE(file.rewind());
  uint32_t size = file.size();
  for (uint32_t i = 0; i < size; i++)
    trace << (char) file.getchar();
  TRACE(file.close());

  MEASURE("Make directory:", 1) CFFS::mkdir("Folder");
  MEASURE("Change current directory:", 1) CFFS::cd("Folder");
  CFFS::ls(trace);
  MEASURE("Change to parent directory:", 1) CFFS::cd("..");
  CFFS::ls(trace);
}

void loop()
{
  ASSERT(true == false);
}
