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

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Types to test
#define UINT_T uint8_t
// #define UINT_T uint16_t
// #define UINT_T uint32_t

// Delay periods to test
static const uint16_t US = 50;
// static const uint16_t US = 100;
// static const uint16_t US = 200;
// static const uint16_t US = 300;
// static const uint16_t US = 1000;

// Start time in micro-seconds; 5 loop cycles before wrap
const uint32_t START = UINT32_MAX - 10000019UL - (US / 2);

// Max number with defined unsigned integer
#define UINT_MAX ((UINT_T) (-1UL))

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaMicros: started"));
  TRACE(UINT_MAX);
  TRACE(US);
  trace.flush();
  RTT::begin();
  RTT::micros(START);
  RTT::micros();
}

void loop()
{
  ASSERT(US < UINT_MAX);
  UINT_T start = RTT::micros();
  DELAY(US);
  UINT_T stop = RTT::micros();
  UINT_T diff = (stop - start);
  trace << RTT::micros() << ':' << stop<< '-' << start << ':' << diff << endl;
  delay(2000);
}

