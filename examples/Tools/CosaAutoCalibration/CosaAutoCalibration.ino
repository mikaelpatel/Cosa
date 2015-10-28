/**
 * @file CosaAutoCalibration.ino
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
 * Calibrate Watchdog clock with RTT as reference. Automatically
 * adjust Watchdog clock to RTT tick.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

RTT::Clock clock;
Watchdog::Clock bark;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaAutoCalibration: started"));

  // Start the watchdog and real-time timer
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  // Measure the watchdog clock. Wait for stable measurement
  uint32_t us = 10000L;
  for (int j = 0; j < 10;) {
    uint32_t t = us;
    uint32_t w = bark.time();
    while (w == bark.time()) yield();
    uint32_t start = RTT::micros();
    w = bark.time();
    while (w == bark.time()) yield();
    uint32_t stop = RTT::micros();
    us = stop - start;
    int32_t diff = t - us;
    if (diff < 0) diff = -diff;
    if (diff > 50) j = 0; else j += 1;
    trace << j << '.';
  }
  trace << endl;
  int32_t diff = (1000000UL - us);
  int16_t ms = (diff + 500L) / 1000;
  bark.adjust(ms);
  trace << PSTR("calibration=") << bark.calibration() << endl;
  sleep(5);
}
