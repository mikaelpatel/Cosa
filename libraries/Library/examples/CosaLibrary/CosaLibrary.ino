/**
 * @file CosaLibrary.ino
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
 * Cosa Library and Component build example sketch.
 *
 * @section Limitations
 * Can only be built with the Cosa build script; cosa. Will not
 * build in the Arduino IDE.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Library.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"

using namespace Library;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaLibrary: started"));
  Watchdog::begin();
  Component comp(1);
}

void loop()
{
  Component comp(2);
  ASSERT(true == false);
}
