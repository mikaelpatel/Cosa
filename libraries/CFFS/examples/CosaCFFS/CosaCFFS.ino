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

#include <CFFS.h>
#include "Cosa/AnalogPin.hh"
#include "Cosa/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/RTT.hh"

#define USE_FLASH_S25FL127S
//#define USE_FLASH_W25X40CL

#if defined(USE_FLASH_S25FL127S) || defined(ANARDUINO_MINIWIRELESS)
#include <S25FL127S.h>
S25FL127S flash;
#endif

#if defined(USE_FLASH_W25X40CL) || defined(WICKEDDEVICE_WILDFIRE)
#include <W25X40CL.h>
W25X40CL flash;
#endif

#define FORMAT_DRIVE
#define WRITE_LOG_ENTRIES

void setup()
{
  // Initiate components and print memory usage
  Watchdog::begin();
  RTT::begin();
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaCFFS: started"));
  TRACE(free_memory());
  TRACE(sizeof(flash));
  TRACE(sizeof(CFFS::File));

  MEASURE("Initiate the flash memory device driver:", 1)
    ASSERT(flash.begin());

#if defined(FORMAT_DRIVE)
  MEASURE("Format file system driver:", 1)
    ASSERT(CFFS::format(&flash, "flash") == 0);
#endif

  MEASURE("Initiate the file system driver:", 1) {
    ASSERT(CFFS::begin(&flash));
  }

  char buf[128];
  int res;
  MEASURE("Reading file system and descriptors:", 1) {
    int res = flash.read(buf, 0L, sizeof(buf));
    ASSERT(res == sizeof(buf));
  }
  trace.print(buf, sizeof(buf), IOStream::hex);

  INFO("List files", 0);
  CFFS::ls(trace);

  INFO("Open and dump file if available", 0);
  CFFS::File file;
  if (file.open("Nisse", O_READ) == 0) {
    uint32_t size = file.size();
    TRACE(size);
    for (uint32_t i = 0; i < size; i++)
      trace << (char) file.getchar();
    TRACE(file.close());
    MEASURE("Open file:", 1)
      ASSERT(file.open("Nisse", O_WRITE) == 0);
    TRACE(file.tell());
  }
  else {
    MEASURE("Open file:", 1)
      ASSERT(file.open("Nisse", O_CREAT | O_EXCL) == 0);
    CFFS::ls(trace);
    TRACE(file.tell());
  }
#if defined(WRITE_LOG_ENTRIES)
  MEASURE("Write 100 log entries:", 1) {
    IOStream cout(&file);
    for (uint16_t i = 0; i < 100; i++) {
      cout << RTT::micros() << PSTR(":A0 = ")
	   << AnalogPin::sample(Board::A0)
	   << endl;
    }
  }
#else
  MEASURE("Write 10 x buf[80]:", 1) {
    memset(buf, ' ', 79);
    buf[79] = '\n';
    for (uint16_t i = 0; i < 10; i++)
      file.write(buf, 80);
  }
#endif
  MEASURE("Tell position:", 1)
    file.tell();
  TRACE(file.tell());

  MEASURE("File size:", 1)
    file.size();
  TRACE(file.size());

  MEASURE("Close file:", 1)
    file.close();

  MEASURE("Open log file:", 1)
    ASSERT(file.open("Nisse", O_READ) == 0);

  TRACE(file.tell());
  MEASURE("Read buffer:", 1)
    res = file.read(buf, sizeof(buf) - 1);
  while (res > 0) {
    buf[res] = 0;
    trace << buf;
    res = file.read(buf, sizeof(buf) - 1);
  }
  TRACE(file.tell());

  INFO("Dump as text to output stream", 0);
  TRACE(file.seek(10, SEEK_SET));
  TRACE(file.seek(1000, SEEK_SET));
  TRACE(file.seek(10000, SEEK_SET));
  TRACE(file.tell());
  bool start = false;
  for (uint16_t i = 0; i < 100; i++) {
    int c = file.getchar();
    if (c == IOStream::EOF) break;
    if (start)
      trace << (char) c;
    else {
      if (c == '\n') start = true;
    }
  }
  TRACE(file.close());

  MEASURE("Make directory:", 1)
    CFFS::mkdir("Folder");
  CFFS::ls(trace);

  MEASURE("Change current directory:", 1)
    CFFS::cd("Folder");
  CFFS::ls(trace);

  TRACE(file.open("Kalle", O_CREAT | O_EXCL));
  CFFS::ls(trace);
  TRACE(file.remove());
  CFFS::ls(trace);
  TRACE(file.open("Kalle", O_CREAT | O_EXCL));
  TRACE(file.write(buf, sizeof(buf)));
  TRACE(file.size());
  TRACE(file.close());
  CFFS::ls(trace);

  MEASURE("Change to parent directory:", 1)
    CFFS::cd("..");
  CFFS::ls(trace);

  TRACE(CFFS::cd("Nisse"));
  TRACE(CFFS::cd("Subfolder"));
  TRACE(CFFS::rmdir("Folder"));
  TRACE(CFFS::mkdir("Folder"));
}

void loop()
{
  ASSERT(true == false);
}
