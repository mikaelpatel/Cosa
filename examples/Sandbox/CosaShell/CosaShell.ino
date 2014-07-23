/**
 * @file CosaShell.ino
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
 * Demonstration of the Cosa Shell commmand line support.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Commands.h"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

IOStream cout;
IOStream cin;

#define LF "\n"

// Blink boot script
const char boot[] __PROGMEM =
  "led on" LF
  "delay 1000" LF
  "led off" LF
  "delay 1000" LF
  "led on" LF
  "delay 1000" LF
  "led off" LF;

void setup()
{
  // Initiate timers
  Watchdog::begin();
  RTC::begin();

  // Initiate UART for blocked read line
  uart.begin(9600);
  uart.set_blocking(SLEEP_MODE_IDLE);
  cin.set_device(&uart);
  cout.set_device(&uart);

  // Run boot script with trace
  cout << PSTR("CosaShell: starting...") << endl;
  shell.script(boot, &cout);
}

void loop()
{
  // The shell command handler will do the top loop
  if (!shell.run(&cin, &cout)) return;
  cout << PSTR("illegal command") << endl;
}
