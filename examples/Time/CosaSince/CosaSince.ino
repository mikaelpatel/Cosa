/**
 * @file CosaSince.ino
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
 * Verify Watchdog::since() and RTC::since() wrap-around behavior.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Clock.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"

RTC::Clock clock;

OutputPin led(Board::LED);

// Start time in milli-seconds
const uint32_t START = 0xfffff000UL;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSince: started"));
  trace.flush();

  // Set timers to the start time
  Watchdog::millis(START);
  RTC::millis(START);

  // Start timers
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  led.on();

  uint32_t rms = RTC::millis();
  uint32_t wms = Watchdog::millis();
  uint32_t wsd = Watchdog::since(START);
  uint32_t rsd = RTC::since(START);
  int32_t diff = wsd - rsd;

  trace << clock.time()
	<< ':' << rms << '-' << rsd
	<< ':' << wms << '-'<< wsd
	<< PSTR(":T") << diff
	<< PSTR(",t") << diff / Watchdog::ms_per_tick()
	<< endl;

  delay(1000 - RTC::since(rms));
  led.off();
  delay(1000);
}

