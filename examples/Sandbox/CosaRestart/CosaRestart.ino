/**
 * @file CosaRestart.ino
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
 * Simple sketch restart. Note: does not reinitiate the internal
 * hardware modules, ports, timers, etc.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"

void (*RESTART)() = NULL;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRestart: started"));
  Watchdog::begin();
  TRACE(Watchdog::millis());
}

void loop()
{
  sleep(4);
  TRACE(Watchdog::millis());
  trace.flush();
  RESTART();
  ASSERT(true == false);
}

