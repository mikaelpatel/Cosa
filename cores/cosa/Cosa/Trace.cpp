/**
 * @file Cosa/Trace.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"

Trace trace;

uint8_t trace_log_mask = LOG_UPTO(LOG_INFO);

bool 
Trace::begin(IOStream::Device* dev, const char* banner)
{
  set_device(dev);
  if (banner != NULL) {
    print_P(banner);
    println();
  }
  return (true);
}

void
Trace::fatal_P(const char* file, int line, const char* expr) 
{
  printf_P(PSTR("%s:%d:%S\n"), file, line, expr);
  print((char) 255);
  get_device()->flush();
  exit(0);
}
