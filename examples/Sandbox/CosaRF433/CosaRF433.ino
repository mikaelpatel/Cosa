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

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"

#if defined(BOARD_ATTINY)
#define PIN Board::D0
#else
#define PIN Board::D6
#endif

OutputPin rf(PIN);

void setup()
{
  RTT::begin();
}

void loop()
{
  const uint16_t D[] = { 1000, 500, 200, 100, 50 };
  for (uint8_t j = 0; j < membersof(D); j++) {
    uint16_t T = D[j];
    rf.set();
    DELAY(2000);
    rf.clear();
    DELAY(2*T);
    uint16_t data = 0x5555;
    uint8_t bit;
    for (uint8_t i = 0; i < 16; i++) {
      bit = data & 1;
      rf.write(bit);
      DELAY(T);
      data >>= 1;
      rf.write(!bit);
      DELAY(T);
    }
    rf.clear();
    sleep(1);
  }
}

