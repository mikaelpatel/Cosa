/**
 * @file CosaGPIO.ino
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
 * Demonstrate Cosa GPIO digital pin access class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/GPIO.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

GPIO led(Board::LED, GPIO::OUTPUT_MODE);
GPIO button(Board::D4, GPIO::INPUT_MODE);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPIO: started"));
  Watchdog::begin();
  TRACE(led.mode());
  TRACE(button.mode());
  button.mode(GPIO::PULLUP_INPUT_MODE);
  TRACE(button.mode());
}

void loop()
{
  if (button) {
    ~led;
    delay(1000);
    ~led;
  }
  delay(1000);
}
