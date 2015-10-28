/**
 * @file CosaIOBuffer.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Cosa IOBuffer demonstration and tests.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOBuffer.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

IOBuffer<16> buffer;

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
  // Check initial state of buffer
  ASSERT(buffer.is_empty());
  ASSERT(!buffer.is_full());
  ASSERT(buffer.available() == 0);
  ASSERT(buffer.room() == 15);

  // Try to peek and get a character when empty
  ASSERT(buffer.peekchar() == IOStream::EOF);
  ASSERT(buffer.peekchar('a') == IOStream::EOF);
  ASSERT(buffer.getchar() == IOStream::EOF);

  // Put a character and check the state
  ASSERT(buffer.putchar('*') == '*');
  ASSERT(!buffer.is_empty());
  ASSERT(!buffer.is_full());
  ASSERT(buffer.available() == 1);
  ASSERT(buffer.room() == 14);

  // Peek again and check that the state did not change
  ASSERT(buffer.peekchar() == '*');
  ASSERT(buffer.peekchar('*') == 1);
  ASSERT(!buffer.is_empty());
  ASSERT(!buffer.is_full());
  ASSERT(buffer.available() == 1);
  ASSERT(buffer.room() == 14);

  // Get again and check the new state
  ASSERT(buffer.getchar() == '*');
  ASSERT(buffer.is_empty());
  ASSERT(!buffer.is_full());
  ASSERT(buffer.available() == 0);
  ASSERT(buffer.room() == 15);

  // Fill the buffer and check the state
  for (char c = 'A'; !buffer.is_full(); c++)
    ASSERT(buffer.putchar(c) == c);
  ASSERT(!buffer.is_empty());
  ASSERT(buffer.is_full());
  ASSERT(buffer.available() == 15);
  ASSERT(buffer.room() == 0);
  ASSERT(buffer.peekchar('A') == 1);
  for (char c = 'A'; !buffer.is_full(); c++)
    ASSERT(buffer.peekchar(c) == c - 'A' + 1);

  // Empty the buffer
  for (char c = 'A'; !buffer.is_empty(); c++)
    ASSERT(buffer.getchar() == c);
  ASSERT(buffer.is_empty());
  ASSERT(!buffer.is_full());
  ASSERT(buffer.available() == 0);
  ASSERT(buffer.room() == 15);

  // Fill the buffer again. Put another character and it should fail
  for (char c = 'A'; !buffer.is_full(); c++)
    ASSERT(buffer.putchar(c) == c);
  ASSERT(buffer.putchar('-') == IOStream::EOF);
  ASSERT(buffer.getchar() == 'A');

  // Scan the buffer
  char s[16];
  ASSERT(buffer.gets(s, sizeof(s)) == s);
  ASSERT(strlen(s) == 14);
  ASSERT(!strcmp_P(s, PSTR("BCDEFGHIJKLMNO")));

  // Scan the buffer
  ASSERT(buffer.putchar('A') == 'A');
  ASSERT(buffer.putchar('B') == 'B');
  ASSERT(buffer.putchar('C') == 'C');
  ASSERT(buffer.putchar('\n') == '\n');
  ASSERT(buffer.putchar('1') == '1');
  ASSERT(buffer.putchar('2') == '2');
  ASSERT(buffer.putchar('3') == '3');
  ASSERT(buffer.putchar('\n') == '\n');

  ASSERT(buffer.gets(s, sizeof(s)) == s);
  ASSERT(strlen(s) == 4);
  ASSERT(!strcmp_P(s, PSTR("ABC\n")));

  ASSERT(buffer.gets(s, sizeof(s)) == s);
  ASSERT(strlen(s) == 4);
  ASSERT(!strcmp_P(s, PSTR("123\n")));

  ASSERT(buffer.gets(s, sizeof(s)) == NULL);
  ASSERT(strlen(s) == 0);

  // Fill the buffer, empty and check state
  for (char c = 'A'; !buffer.is_full(); c++)
    ASSERT(buffer.putchar(c) == c);
  buffer.empty();
  ASSERT(buffer.is_empty());

  // End the test suite
  ASSERT(true == false);
}

