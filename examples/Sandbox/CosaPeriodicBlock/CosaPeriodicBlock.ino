/**
 * @file CosaPeriodicBlock.ino
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
 * Demo of Cosa Periodic Blocks.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPeriodicBlock: started"));
  RTT::begin();
}

void loop()
{
  static uint16_t nr = 0;
  static uint16_t x = 0;
  static uint16_t y = 0;
  static uint16_t z = 0;

  delay(10);
  nr += 1;

  periodic(t1, 100) {
    x += 1;
  }

  periodic(t2, 1000) {
    y += 1;
  }

  periodic(t3, 10000) {
    z += 1;
  }

  periodic(t4, 5000) {
    trace << RTT::millis()
	  << ' ' << RTT::since(t4)
	  << ' ' << nr
	  << ' ' << x
	  << ' ' << y
	  << ' ' << z
	  << endl;
  }
}

