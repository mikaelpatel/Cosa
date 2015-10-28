/**
 * @file CosaPSTR.ino
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
 * Testing code generation of string pointers to program memory.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/String.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPSTR: started"));
  Watchdog::begin();
}

static const char str0[] __PROGMEM = "Nisse badar";
static const str_P str1 = (str_P) str0;

void loop()
{
  str_P str2 = PSTR("Nisse badar");
  const char* str3 = "Nisse badar";
  String str4;

  trace << (void*) str0 << ':' << (str_P) str0 << endl;
  trace << (void*) str1 << ':' << str1 << endl;
  trace << (void*) str2 << ':' << str2 << endl;
  trace << (void*) str3 << ':' << str3 << endl;

  str4 = (str_P) str0;
  trace << (void*) str4.c_str() << ':' << str4 << endl;

  str4 = str1;
  trace << (void*) str4.c_str() << ':' << str4 << endl;

  str4 = str2;
  trace << (void*) str4.c_str() << ':' << str4 << endl;

  str4 = str3;
  trace << (void*) str4.c_str() << ':' << str4 << endl;

  ASSERT(true == false);
}
