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
 * See Commands.cpp for the implementation of the toy Arduino 
 * shell.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Commands.h"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Input and output streams
IOStream cin;
IOStream cout;

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

  // Run blink command (which is actually a script)
  cout << PSTR("CosaShell: starting...") << endl;
  shell.execute_P(PSTR("blink"));
}

void loop()
{
  // The shell command handler will do the top loop
  if (!shell.run(&cin, &cout)) return;
  cout << PSTR("illegal command") << endl;
}
