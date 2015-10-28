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
 * | ..... |
 * | CHAN7 |-------------------------------> D1/TX
 * |       |
 * | GND   |-------------------------------> GND
 * +-------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/UART.hh"

OutputPin led(Board::LED);

void setup()
{
  // Print short info about the logic analyser probe channels
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalyzerFlush: started"));
  trace << PSTR("CHAN0 - D13/LED [^]") << endl;
  trace << PSTR("CHAN7 - D1/TX [Async Serial]") << endl;
  trace.flush();

  // Use the RTT for delay
  RTT::begin();
}

void loop()
{
  // Toggle led for baseline
  RTT::await();
  led.on();
  led.off();

  // Write to output buffer
  RTT::await();
  led.on();
  trace << PSTR("hello ");
  led.off();

  // Write again to output buffer and wait for completion
  RTT::await();
  led.on();
  trace << PSTR("world");
  trace.flush();
  led.off();
  trace << endl;
  trace.flush();
  led.on();
  led.off();

  // Take a nap
  sleep(2);
}

