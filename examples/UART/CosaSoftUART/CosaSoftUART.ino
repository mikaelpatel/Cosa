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

OutputPin led(Board::LED);
IOBuffer<Soft::UART::RX_BUFFER_MAX> ibuf;

#if defined(BOARD_ATTINY)
// Arduino Tiny: RX(D2), TX(D1)
Soft::UART uart(Board::D2, Board::PCI1, &ibuf);
#define BAUDRATE 38400
#elif defined(BOARD_ATMEGA2560)
// Arduino Mega: RX(D11), TX(D10)
Soft::UART uart(Board::D11, Board::PCI4, &ibuf);
#define BAUDRATE 57600
#else
// Arduino Standard: RX(D5), TX(D4)
Soft::UART uart(Board::D5, Board::PCI4, &ibuf);
#define BAUDRATE 57600
#endif

void setup()
{
  Watchdog::begin();
  uart.begin(BAUDRATE);
  uart.puts(PSTR("Hello World\n"));
}

void loop()
{
  // Echo characters received on soft uart
  uint32_t start = Watchdog::millis();
  while (Watchdog::since(start) < 2000) {
    while (uart.available()) uart.putchar(uart.getchar());
   }

  // Write pin status to soft uart
  led.toggle();
  uint8_t max = membersof(digital_pin_map);
  uart.puts(PSTR("D[0.."));
  if (max < 10) {
    uart.putchar(max + '0');
  }
  else {
    uart.putchar(max/10 + '0');
    uart.putchar(max%10 + '0');
  }
  uart.puts(PSTR("]:"));
  for (uint8_t ix = 0; ix < membersof(digital_pin_map); ix++) {
    Board::DigitalPin pin;
    pin = (Board::DigitalPin) pgm_read_byte(digital_pin_map + ix);
    uart.putchar(Pin::read(pin) ? '1' : '0');
  }
  uart.putchar('\n');
  led.toggle();
}
