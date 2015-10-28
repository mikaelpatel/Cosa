/**
 * @file CosaDS18B20calc.ino
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
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS18B20calc: started"));
}

int32_t iscale(int16_t temp)
{
  bool negative = false;
  if (temp < 0) {
    temp = -temp;
    negative = true;
  }
  int32_t res = (temp >> 4) * 1000L + (625 * (temp & 0xf));
  return (negative ? -res : res);
}

float32_t fscale(int16_t temp)
{
  return (temp * 0.0625);
}

#define CHECK(c,t) trace << PSTR(#c "=") << fscale(t) << endl

void loop()
{
  CHECK(+125, 0x07D0);
  CHECK(+85, 0x0550);
  CHECK(+25.0625, 0x0191);
  CHECK(+10.125, 0x00A2);
  CHECK(+0.5, 0x0008);
  CHECK(0, 0x0000);
  CHECK(-0.5, 0xFFF8);
  CHECK(-10.125, 0xFF5E);
  CHECK(-25.0625, 0xFE6F);
  CHECK(-55, 0xFC90);
  ASSERT(true == false);
}
