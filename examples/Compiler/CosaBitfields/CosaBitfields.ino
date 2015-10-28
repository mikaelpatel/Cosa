/**
 * @file CosaBitfields.ino
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
 * Evaluate different methods of bit-field access using macro, enum,
 * shift and mask, and bit-fields struct definitions. Check assembly
 * listing.
 *
 * @section Circuit
 * This example does not require any special circuit. Uses UART and
 * Watchdog.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

#include "color16.h"
#include "config.h"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBitfields: started"));
  TRACE(free_memory());
  Watchdog::begin();
}

void loop()
{
  static uint8_t red, green, blue = 0;
  static uint8_t rate = 0x00;

  color16_t foreground;
  foreground.red = red;
  foreground.green = green;
  foreground.blue = blue;
  trace << PSTR("foreground = ") << bin << foreground.rgb << endl;
  trace << PSTR("red = ") << bin << foreground.red << endl;
  trace << PSTR("green = ") << bin << foreground.green << endl;
  trace << PSTR("blue = ") << bin << foreground.blue << endl;

  trace << PSTR("color16a = ") << bin << color16a(red, green, blue) << endl;
  trace << PSTR("color16b = ") << bin << color16b(red, green, blue) << endl;

  config_t config;
  config.bias = POSITIVE_BIAS;
  config.rate = (rate_t) rate;
  config.avg = SAMPLES_AVG_4;
  trace << PSTR("config = ") << bin << config.reg << endl;
  trace << PSTR("bias = ") << bin << config.bias << endl;
  trace << PSTR("rate = ") << bin << config.rate << endl;
  trace << PSTR("avg = ") << bin << config.avg << endl;

  red += 1;
  green += 1;
  blue += 1;
  rate += 1;
  sleep(4);
}
