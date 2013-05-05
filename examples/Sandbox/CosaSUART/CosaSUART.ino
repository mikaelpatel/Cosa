/**
 * @file CosaSUART.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * @section Description
 * Cosa demonstration of Soft UART for the ATtinyX5.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

OutputPin led(Board::LED);
#undef putchar

void setup()
{
  Watchdog::begin();
  uart.begin(9600);
  uart.puts_P(PSTR("Hello World\n"));
}

void loop()
{
  static uint8_t pin = 0;
  led.toggle();
  uart.putchar('D');
  if (pin < 10) {
    uart.putchar(pin + '0');
  }
  else {
    uart.putchar('1');
    uart.putchar(pin + '0' - 10);
  }
  if (Pin::read(pin))
    uart.puts_P(PSTR(" = on\n"));
  else uart.puts_P(PSTR(" = off\n"));
  led.toggle();
  SLEEP(1);
  if (pin == Board::PIN_MAX) pin = 0; else pin += 1;
}
