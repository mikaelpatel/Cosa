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
#include "Cosa/Tone.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Input and output streams
IOStream cin;
IOStream cout;

void setup()
{
  // Initiate timers
  Watchdog::begin();
  Tone::begin();
  RTC::begin();
  
  // Initiate UART for blocked read line
  uart.begin(9600);
  uart.set_blocking();
  cin.set_device(&uart);
  cout.set_device(&uart);
}

void loop()
{
  // The shell will do the read and execute of commands
  shell.run(&cin, &cout);
}
