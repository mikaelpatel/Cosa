/**
 * @file CosaSoftUART.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Cosa demonstration of Software UART and using the raw
 * IOStream::Device interface.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Soft/UART.hh"

IOBuffer<Soft::UART::RX_BUFFER_MAX> ibuf;
#if defined(BOARD_ATTINY)
Soft::UART uart(Board::D2, Board::PCI1, &ibuf);
#define BAUDRATE 38400
#else
Soft::UART uart(Board::D5, Board::PCI4, &ibuf);
#define BAUDRATE 57600
#endif
OutputPin led(Board::LED);

void setup()
{
  Watchdog::begin();
  uart.begin(BAUDRATE);
  uart.puts(PSTR("Hello World\n"));
}

void loop()
{
  static uint8_t ix = 0;

  // Write pin status to soft uart
  led.toggle();
  uart.putchar('D');
  if (ix < 10) {
    uart.putchar(ix + '0');
  }
  else {
    uart.putchar(ix/10 + '0');
    uart.putchar(ix%10 + '0');
  }
  Board::DigitalPin pin;
  pin = (Board::DigitalPin) pgm_read_byte(digital_pin_map + ix);
  if (Pin::read(pin))
    uart.puts(PSTR(" = on\n"));
  else uart.puts(PSTR(" = off\n"));
  led.toggle();
  if (ix == membersof(digital_pin_map)) ix = 0; else ix += 1;

  // Echo characters received on soft uart
  while (uart.available()) uart.putchar(uart.getchar());
  sleep(1);
}
