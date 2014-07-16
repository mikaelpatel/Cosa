/**
 * @file CosaCFFS.ino
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
 * Demonstration the S25FL127S SPI Flash Memory device driver and file
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

S25FL127S flash;

// Disable radio module on common spi bus
#if defined(ANARDUINO_MINIWIRELESS)
OutputPin rf_cs(Board::D10, 1);
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

  INFO("Initiate the flash memory device driver and file system driver", 0);
  ASSERT(flash.begin());
  if (!CFFS::begin(&flash)) {
    ASSERT(!CFFS::format(&flash));
    ASSERT(CFFS::begin(&flash));
  }

  INFO("List files; default and verbose mode", 0);
  CFFS::ls(trace);
  CFFS::ls(trace, true);

  INFO("Open log file and write a new sample", 0);
  CFFS::File file;
  if (file.open("Kalle", O_CREAT | O_EXCL) != 0)
    ASSERT(file.open("Kalle", O_WRITE) == 0);
  TRACE(file.tell());
  IOStream cout(&file);
  cout << RTC::micros() << PSTR(":A0 = ") 
       << AnalogPin::sample(Board::A0)
       << endl;
  TRACE(file.tell());
  TRACE(file.size());
  TRACE(file.close());
  
  INFO("Open log file again and dump binary", 0);
  ASSERT(file.open("Kalle", O_READ) == 0);
  char buf[32];
  while (file.read(buf, sizeof(buf)) > 0) 
    trace.print(buf, sizeof(buf), IOStream::hex);
  TRACE(file.read(buf, sizeof(buf)));

  INFO("Rewind and dump in text", 0);
  TRACE(file.rewind());
  uint32_t size = file.size();
  for (uint32_t i = 0; i < size; i++)
    trace << (char) file.getchar();
  TRACE(file.close());

  INFO("Create a folder and change current directory", 0);
  TRACE(CFFS::mkdir("Folder"));
  TRACE(CFFS::cd("Folder"));

  INFO("List files and change back to parent directory", 0);
  CFFS::ls(trace);
  TRACE(CFFS::cd(".."));
  CFFS::ls(trace);
}

void loop()
{
  ASSERT(true == false);
}
