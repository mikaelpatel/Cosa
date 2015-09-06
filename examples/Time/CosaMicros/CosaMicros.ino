/**
 * @file CosaMicros.ino
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
 * Verify wrap around behavior of unsigned integer based micro-seconds
 * calculations.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Types to test
#define UINT_T uint8_t
// #define UINT_T uint16_t
// #define UINT_T uint32_t

// Delay periods to test
// static const uint16_t US = 50;
// static const uint16_t US = 100;
static const uint16_t US = 200;
// static const uint16_t US = 300;
// static const uint16_t US = 1000;

// Start time in micro-seconds; 5 loop cycles before wrap
const uint32_t START = UINT32_MAX - 10000019UL - (US / 2);

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaMicros: started"));
  trace.flush();
  RTC::begin();
  RTC::micros(START);
  RTC::micros();
}

void loop()
{
  UINT_T start = RTC::micros();
  DELAY(US);
  UINT_T stop = RTC::micros();
  UINT_T diff = (stop - start);
  trace << RTC::micros() << ':' << diff << endl;
  delay(2000);
}

