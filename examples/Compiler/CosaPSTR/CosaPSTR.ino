/**
 * @file CosaPSTR.ino
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Demonstrate common PSTR sharing. Compiler will find equal
 * string and substrings.
 *
 * Shared: 	
 *   Sketch uses 5,768 bytes (18%) of program storage space.
 * Non-shared:
 *   Sketch uses 5,800 bytes (18%) of program storage space.
 * Saved: 32 bytes 
 *   Single "true", saved 10
 *   Single "false", saved 12
 *   Single "xxxx", saved 9
 *   
 * @section Acknowledgements
 * Inspired by Michael Buschbeck, "Your fla-Your flash is now clean"
 * http://michael-buschbeck.github.io/arduino/2013/10/20/string-merging-pstr/
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPSTR: started"));
  Watchdog::begin();

  trace << PSTR("true") << endl;
  trace << PSTR("true") << endl;
  trace << PSTR("true") << endl;

  trace << PSTR("false") << endl;
  trace << PSTR("false") << endl;
  trace << PSTR("false") << endl;
  
  const char* x4 = PSTR("xxxx");
  const char* x3 = PSTR("xxx");
  const char* x2 = PSTR("xx");
  const char* x1 = PSTR("x");

  trace << (void*) x4 << ':' << x4 << endl;
  trace << (void*) x3 << ':' << x3 << endl;
  trace << (void*) x2 << ':' << x2 << endl;
  trace << (void*) x1 << ':' << x1 << endl;

  uint8_t x = 13;
  trace << PSTR("x = ") << hex << x << endl;
  TRACE(x);
  INFO("x = %d", x);
}

void loop()
{
  ASSERT(true == false);
}
