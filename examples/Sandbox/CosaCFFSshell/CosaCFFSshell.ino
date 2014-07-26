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

#include "Commands.h"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/SPI/Driver/S25FL127S.hh"
#include "Cosa/FS/CFFS.hh"

#if defined(ANARDUINO_MINIWIRELESS)
OutputPin rf_cs(Board::D10, 1);
#endif

S25FL127S flash;
IOStream cout;
IOStream cin;

void setup()
{
  // Initiate timers
  Watchdog::begin();
  RTC::begin();

  // Initiate UART for blocked read line
  uart.begin(9600);
  uart.set_blocking();
  cin.set_device(&uart);
  cout.set_device(&uart);

  // Initiate flash memory and file system
  if (flash.begin() && CFFS::begin(&flash)) return;
  cout << PSTR("no flash memory") << endl;
  exit(0);
}

void loop()
{
  // The shell command handler will do the top loop
  shell.run(&cin, &cout);
}
