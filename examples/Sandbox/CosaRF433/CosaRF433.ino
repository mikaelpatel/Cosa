/**
 * @file CosaRF433.ino
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
 * Study RF433 AGC behavior.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"

OutputPin rf(Board::D6);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRF433: started"));
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  const uint16_t D[] = { 1000, 500, 200, 100, 50 };
  rf.set();
  delay(10);
  rf.clear();
  for (uint8_t j = 0; j < membersof(D); j++) {
    uint16_t T = D[j];
    for (uint8_t i = 0; i < 8; i++) {
      DELAY(T);
      rf.set();
      DELAY(T);
      rf.clear();
    }
  }
  sleep(2);
}

