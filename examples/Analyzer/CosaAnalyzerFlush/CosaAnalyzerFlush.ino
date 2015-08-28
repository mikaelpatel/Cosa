/**
 * @file CosaAnalyzerFlush.ino
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
 * Logic Analyzer based analysis of IOStream/UART flush behavior.
 *
 * @section Circuit
 * Trigger on CHAN0/D13/LED rising.
 *
 * +-------+
 * | CHAN0 |-------------------------------> D13/LED
 * | CHAN1 |-------------------------------> D1/TX
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"

OutputPin led(Board::LED, 0);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerFlush: started"));
  Watchdog::begin();
}

void loop()
{
  led.on();
  trace << PSTR("hello world") << endl;
  trace.flush();
  led.off();
  sleep(2);
}

