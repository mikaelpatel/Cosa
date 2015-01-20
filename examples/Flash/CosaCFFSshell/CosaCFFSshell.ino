/**
 * @file CosaCFFSshell.ino
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
 * Tiny command shell to demonstration the S25FL127S SPI Flash Memory
 * device driver and file system.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "CFFScommands.h"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/FS/CFFS.hh"

//#define USE_FLASH_S25FL127S
//#define USE_FLASH_W25X40CL

#if defined(USE_FLASH_S25FL127S) || defined(ANARDUINO_MINIWIRELESS)
#include "Cosa/Flash/Driver/S25FL127S.hh"
S25FL127S flash;
#endif

#if defined(USE_FLASH_W25X40CL) || defined(WICKEDDEVICE_WILDFIRE)
#include "Cosa/Flash/Driver/W25X40CL.hh"
W25X40CL flash;
#endif

IOStream ios(&uart);

void setup()
{
  // Initiate timers
  Watchdog::begin();
  RTC::begin();

  // Initiate UART for blocked read line
  uart.begin(9600);

  // Initiate flash memory and file system
  if (flash.begin() && CFFS::begin(&flash)) return;
  ios << PSTR("no flash memory") << endl;
  exit(0);
}

void loop()
{
  // The shell command handler will do the top loop
  shell.run(ios);
}
