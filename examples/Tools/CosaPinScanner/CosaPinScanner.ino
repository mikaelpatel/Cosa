/**
 * @file CosaPinScanner.ino
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
 * @section Description
 * Periodically scan pins.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinScanner: started"));
  Watchdog::begin();
  SLEEP(5);
}

void print(IOStream& outs, uint8_t value)
{
  uint8_t bit = 0x80;
  while (bit) {
    outs << ((value & bit) != 0);
    bit >>= 1;
  }
  outs << ' ';
}

void loop()
{
#if defined(PINA)
  trace << PSTR("PINA     ");
#endif
#if defined(PINB)
  trace << PSTR("PINB     ");
#endif
#if defined(PINC)
  trace << PSTR("PINC     ");
#endif
#if defined(PIND)
  trace << PSTR("PIND     ");
#endif
#if defined(PINE)
  trace << PSTR("PINE     ");
#endif
#if defined(PINF)
  trace << PSTR("PINF     ");
#endif
#if defined(PING)
  trace << PSTR("PING     ");
#endif
#if defined(PINH)
  trace << PSTR("PINH     ");
#endif
  trace << endl;
#if defined(PINA)
  print(trace, PINA);
#endif
#if defined(PINB)
  print(trace, PINB);
#endif
#if defined(PINC)
  print(trace, PINC);
#endif
#if defined(PIND)
  print(trace, PIND);
#endif
#if defined(PINE)
  print(trace, PINE);
#endif
#if defined(PINF)
  print(trace, PINF);
#endif
#if defined(PING)
  print(trace, PING);
#endif
#if defined(PINH)
  print(trace, PINH);
#endif
  trace << endl;
  SLEEP(2);
}

