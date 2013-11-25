/**
 * @file Cosa/Trace.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
Trace::fatal_P(const char* expr, int line, const char* func)
{
  printf_P(PSTR("%d:%s:%S"), line, func, expr);
  get_device()->flush();
  exit(0);
}
