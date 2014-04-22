/**
 * @file CosaSoftUART.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Cosa demonstration of Software UART and using the raw 
 * IOStream::Device interface. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Soft/UART.hh"

IOBuffer<Soft::UART::BUFFER_MAX> ibuf;
#if defined(__ARDUINO_TINY__)
Soft::UART uart(Board::D2, Board::PCI1, &ibuf);
#define BAUDRATE 19200
#else
Soft::UART uart(Board::D5, Board::PCI4, &ibuf);
#define BAUDRATE 57600
#endif
OutputPin led(Board::LED);

void setup()
{
  Watchdog::begin();
  uart.begin(BAUDRATE);
  uart.puts_P(PSTR("Hello World\n"));
}

void loop()
{
  static uint8_t pin = 0;

  // Write pin status to soft uart
  led.toggle();
  uart.putchar('D');
  if (pin < 10) {
    uart.putchar(pin + '0');
  }
  else {
    uart.putchar(pin/10 + '0');
    uart.putchar(pin%10 + '0');
  }
  if (Pin::read(pin))
    uart.puts_P(PSTR(" = on\n"));
  else uart.puts_P(PSTR(" = off\n"));
  led.toggle();
  if (pin == Board::PIN_MAX) pin = 0; else pin += 1;

  // Echo characters received on soft uart
  while (uart.available()) uart.putchar(uart.getchar());
  SLEEP(1);
}
