/**
 * @file CosaACS712T.ino
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
 * Study ACS712T Hall Effect-Based Linear Current Sensor behavior.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/UART.hh"

AnalogPin sensor(Board::A0);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaACS712T: started"));
  Watchdog::begin();
}

void loop()
{
  // float32_t current = -5.0 + (sensor / 102.3);
  const uint16_t SAMPLE_MAX = 16;
  uint16_t value = 0;
  for (uint8_t i = 0; i < SAMPLE_MAX; i++)
    value += sensor;
  int16_t current = -5000 + ((10000UL * (value / SAMPLE_MAX)) / 1023);
  trace << (value / SAMPLE_MAX) << ':' << current << PSTR(" mA") << endl;
  sleep(2);
}

