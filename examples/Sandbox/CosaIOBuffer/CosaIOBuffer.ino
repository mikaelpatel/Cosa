/**
 * @file CosaIOBuffer.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Cosa IOBuffer demonstration and tests.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOBuffer.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

IOBuffer<16> buffer;
#undef putchar
#undef getchar

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaIOBuffer: started"));
  TRACE(sizeof(uart));
  TRACE(sizeof(trace));
  TRACE(sizeof(buffer));
}

void loop()
{

  INFO("Check initial state of buffer", 0);
  TRACE(buffer.is_empty());
  TRACE(buffer.is_full());
  TRACE(buffer.available());
  TRACE(buffer.room());

  INFO("Try to peek and get a character when empty", 1);
  TRACE(buffer.peekchar());
  TRACE(buffer.getchar());

  INFO("Put a character and check the state", 2);
  TRACE(buffer.putchar('*'));
  TRACE(buffer.is_empty());
  TRACE(buffer.is_full());
  TRACE(buffer.available());
  TRACE(buffer.room());

  INFO("Peek again and check that the state did not change", 3);
  TRACE(buffer.peekchar());
  TRACE(buffer.is_empty());
  TRACE(buffer.is_full());
  TRACE(buffer.available());
  TRACE(buffer.room());

  INFO("Get again and check the new state", 4);
  TRACE(buffer.getchar());
  TRACE(buffer.is_empty());
  TRACE(buffer.is_full());
  TRACE(buffer.available());
  TRACE(buffer.room());

  INFO("Fill the buffer and check the state", 5);
  for (char c = 'A'; !buffer.is_full(); c++) TRACE(buffer.putchar(c));
  TRACE(buffer.is_empty());
  TRACE(buffer.is_full());
  TRACE(buffer.available());
  TRACE(buffer.room());

  INFO("Empty the buffer", 6);
  while (buffer.available()) trace << (char) buffer.getchar();
  trace << endl;
  TRACE(buffer.is_empty());
  TRACE(buffer.is_full());
  TRACE(buffer.available());
  TRACE(buffer.room());

  ASSERT(true == false);
}

