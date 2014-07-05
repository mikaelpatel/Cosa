/**
 * @file CosaString.ino
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
 * Demonstrate how to use Arduino/Wiring String class in Cosa.
 * Please note that the include order to get IOStream output 
 * operator for String.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/String.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaString: started"));
  Watchdog::begin();
}

void loop()
{
  TRACE(free_memory());

  static uint16_t count = 1000;
  String s = "Nisse badar";
  trace << s << endl;

  s += F(" i svartvitt");
  trace << s << endl;

  s += F(" till ");
  s += count++;
  trace << s << endl;

  sleep(2);
}
