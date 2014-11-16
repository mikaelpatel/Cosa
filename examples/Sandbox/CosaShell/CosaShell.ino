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
 * Demonstration of the Cosa Shell commmand line support. See
 * Commands.cpp for the implementation of the toy Arduino shell.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Commands.h"
#include "Cosa/RTC.hh"
#include "Cosa/Tone.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  Watchdog::begin();
  Tone::begin();
  RTC::begin();
  uart.begin(57600);
  yield = iowait;
}

void loop()
{
  shell.run(ios);
  yield();
}

