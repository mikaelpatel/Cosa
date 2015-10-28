/**
 * @file CosaDebug.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Cosa Debug demonstration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

// Define to remove debug statements
// #define NDEBUG

#include <Debug.h>
#include "Cosa/UART.hh"

namespace A {

  // Single breakpoint
  void a()
  {
    BREAKPOINT();
  }

  // Multiple breakpoints with call between
  void b()
  {
    BREAKPOINT();
    a();
    BREAKPOINT();
  }

  // Register variable, conditional observation and breakpoint
  void c()
  {
    static int i = 0;
    REGISTER(i);
    OBSERVE_IF(i < 1, i);
    OBSERVE_IF(i > 5, i);
    BREAK_IF(i == 10);
    i += 1;
    ASSERT(i < 15);
  }

  long e(int i)
  {
    REGISTER(i);
    CHECK_STACK();
    if (i != 0) return (e(i - 1) * i);
    return (1);
  }
};

IOStream cout(&uart);
const size_t BUF_MAX = 128;
char* buf = NULL;

void setup()
{
  // Set the debug stream
  uart.begin(9600);
  DEBUG_STREAM(uart);

  // Register global data (for this scope)
  REGISTER(buf);
  cout << PSTR("setup running") << endl;

  // Contains a breakpoint. Check the memory. No heap used
  A::a();

  // Allocate from heap
  buf = (char*) malloc(BUF_MAX);
  memset(buf, 0xa5, BUF_MAX);

  // Contains several breakpoints. Check the memory again. Heap is now used
  A::b();

  // Free allocated buffer. Check memory and heap again
  free(buf);
  A::a();
}

void loop()
{
  // Local variable
  uint8_t i = 123;

  // Register all the variables the debug handler should know about
  REGISTER(buf);
  REGISTER(BUF_MAX);
  REGISTER(cout);
  REGISTER(uart);
  REGISTER(i);

  cout << PSTR("loop running") << endl;

  // Contains both breakpoints and observations. Check variables
  A::c();

  // Leak memory and run function with memory check
  buf = (char*) malloc(BUF_MAX);
  memset(buf, 0xa5, BUF_MAX);
  cout << A::e(5) << endl;

  // Keep up with the user
  delay(500);
}

