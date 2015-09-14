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
 * Calibrate Watchdog clock with RTC clock as reference. Automatically
 * adjust Watchdog clock to RTC clock tick.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

RTC::Clock clock;
Watchdog::Clock bark;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaAutoCalibration: started"));

  // Start the watchdog and internal real-time clock
  Watchdog::begin();
  RTC::begin();

  // Synchronized clocks
  uint32_t now = clock.await();
  delay(500);
  bark.time(now + 1);
}

void loop()
{
  static int32_t cycle = 1;

  // Wait for clock update
  uint32_t now = clock.await();

  // Calculate error and possible adjustment
  int32_t diff = bark.time() - now;
  int32_t err = (1000 * diff) / cycle;
  if (err != 0) {
    bark.adjust(err / 2);
    trace << endl << PSTR("calibration=") << bark.calibration() << endl;
    cycle = 1;
    clock.time(0);
    now = clock.await();
    delay(500);
    bark.time(now + 1);
  }
  else {
    trace << '.';
    cycle += 1;
  }
}
