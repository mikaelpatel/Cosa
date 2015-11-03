/**
 * @file CosaTCS230.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Demonstrate Cosa TCS230 Programmable Color Light-to-Frequency
 * Converter device driver.
 *
 * @section Circuit
 * @code
 *                       TCS230 Module
 *                   P1 +------------+ P2
 * (D4)---------------1-|S0        S3|-1-----------------(D7)
 * (D5)---------------2-|S1        S2|-2-----------------(D6)
 *                    3-|OE       OUT|-3------------(D3/EXT1)
 * (GND)--------------4-|GND      VCC|-4----------------(VCC)
 *                      +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <TCS230.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"
#include "Cosa/Trace.hh"

TCS230 sensor;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTCS230: started"));
  Watchdog::begin();
  RTT::begin();
  sensor.frequency_scaling(100);
}

void loop()
{
  sensor.photodiode(TCS230::NO_FILTER);
  uint16_t clear = sensor.sample();

  sensor.photodiode(TCS230::RED_FILTER);
  uint16_t red = sensor.sample();

  sensor.photodiode(TCS230::GREEN_FILTER);
  uint16_t green = sensor.sample();

  sensor.photodiode(TCS230::BLUE_FILTER);
  uint16_t blue = sensor.sample();

  trace << clear << ':' << red << ',' << green << ',' << blue;
  if (clear == UINT16_MAX) trace << PSTR(":overflow");
  trace << endl;

  sleep(1);
}
