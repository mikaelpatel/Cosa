/**
 * @file CosaLCDterm.ino
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
 * @section Description
 * Cosa LCD demo with UART echo.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/LCD/Driver/HD44780.hh"

// Select LCD and Port
// HD44780::Port4b port;
// HD44780::Port4p port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4 port;
// HD44780::MJKDZ port;
HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780 lcd(&port, 20, 4);
HD44780 lcd(&port);

void setup()
{
  Watchdog::begin();
  uart.begin(9600);
  lcd.begin();
}

void loop()
{
  while (uart.available()) lcd.putchar(uart.getchar());
  Power::sleep(SLEEP_MODE_IDLE);
}
