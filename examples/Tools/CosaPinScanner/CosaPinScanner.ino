/**
 * @file CosaPinScanner.ino
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
 * Periodically scan pins.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"
#include "Cosa/Trace.hh"

void print(IOStream& outs, uint8_t value)
{
  uint8_t bit = 0x80;
  while (bit) {
    outs << ((value & bit) != 0);
    bit >>= 1;
  }
  outs << ' ';
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinScanner: started"));
  Watchdog::begin();
#if defined(DDRA)
  trace << PSTR("DDRA     ");
#endif
#if defined(DDRB)
  trace << PSTR("DDRB     ");
#endif
#if defined(DDRC)
  trace << PSTR("DDRC     ");
#endif
#if defined(DDRD)
  trace << PSTR("DDRD     ");
#endif
#if defined(DDRE)
  trace << PSTR("DDRE     ");
#endif
#if defined(DDRF)
  trace << PSTR("DDRF     ");
#endif
#if defined(DDRG)
  trace << PSTR("DDRG     ");
#endif
#if defined(DDRH)
  trace << PSTR("DDRH     ");
#endif
#if defined(DDRI)
  trace << PSTR("DDRI     ");
#endif
#if defined(DDRJ)
  trace << PSTR("DDRJ     ");
#endif
#if defined(DDRJ)
  trace << PSTR("DDRK     ");
#endif
#if defined(DDRJ)
  trace << PSTR("DDRL     ");
#endif
  trace << endl;
#if defined(DDRA)
  print(trace, DDRA);
#endif
#if defined(DDRB)
  print(trace, DDRB);
#endif
#if defined(DDRC)
  print(trace, DDRC);
#endif
#if defined(DDRD)
  print(trace, DDRD);
#endif
#if defined(DDRE)
  print(trace, DDRE);
#endif
#if defined(DDRF)
  print(trace, DDRF);
#endif
#if defined(DDRG)
  print(trace, DDRG);
#endif
#if defined(DDRH)
  print(trace, DDRH);
#endif
#if defined(DDRI)
  print(trace, DDRI);
#endif
#if defined(DDRJ)
  print(trace, DDRJ);
#endif
#if defined(DDRK)
  print(trace, DDRK);
#endif
#if defined(DDRL)
  print(trace, DDRL);
#endif
  trace << endl;
#if defined(PORTA)
  trace << PSTR("PORTA    ");
#endif
#if defined(PORTB)
  trace << PSTR("PORTB    ");
#endif
#if defined(PORTC)
  trace << PSTR("PORTC    ");
#endif
#if defined(PORTD)
  trace << PSTR("PORTD    ");
#endif
#if defined(PORTE)
  trace << PSTR("PORTE    ");
#endif
#if defined(PORTF)
  trace << PSTR("PORTF    ");
#endif
#if defined(PORTG)
  trace << PSTR("PORTG    ");
#endif
#if defined(PORTH)
  trace << PSTR("PORTH    ");
#endif
#if defined(PORTI)
  trace << PSTR("PORTI    ");
#endif
#if defined(PORTJ)
  trace << PSTR("PORTJ    ");
#endif
#if defined(PORTJ)
  trace << PSTR("PORTK    ");
#endif
#if defined(PORTJ)
  trace << PSTR("PORTL    ");
#endif
  trace << endl;
#if defined(PORTA)
  print(trace, PORTA);
#endif
#if defined(PORTB)
  print(trace, PORTB);
#endif
#if defined(PORTC)
  print(trace, PORTC);
#endif
#if defined(PORTD)
  print(trace, PORTD);
#endif
#if defined(PORTE)
  print(trace, PORTE);
#endif
#if defined(PORTF)
  print(trace, PORTF);
#endif
#if defined(PORTG)
  print(trace, PORTG);
#endif
#if defined(PORTH)
  print(trace, PORTH);
#endif
#if defined(PORTI)
  print(trace, PORTI);
#endif
#if defined(PORTJ)
  print(trace, PORTJ);
#endif
#if defined(PORTK)
  print(trace, PORTK);
#endif
#if defined(PORTL)
  print(trace, PORTL);
#endif
  trace << endl;
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
#if defined(PINI)
  trace << PSTR("PINI     ");
#endif
#if defined(PINJ)
  trace << PSTR("PINJ     ");
#endif
#if defined(PINK)
  trace << PSTR("PINK     ");
#endif
#if defined(PINL)
  trace << PSTR("PINL     ");
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
#if defined(PINI)
  print(trace, PINI);
#endif
#if defined(PINJ)
  print(trace, PINJ);
#endif
#if defined(PINK)
  print(trace, PINK);
#endif
#if defined(PINL)
  print(trace, PINL);
#endif
  trace << endl;
  sleep(2);
}

