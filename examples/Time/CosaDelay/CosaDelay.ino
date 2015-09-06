/**
 * @file CosaDelay.ino
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
 * Test RTC and Watchdog delay wrap around behavior.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Configuration
// #define USE_RTC
#define USE_WATCHDOG

#if defined(USE_RTC)
#include "Cosa/RTC.hh"
#define TIMER RTC
#define NAME "RTC"
#endif

#if defined(USE_WATCHDOG)
#include "Cosa/Watchdog.hh"
#define TIMER Watchdog
#define NAME "Watchdog"
#endif

// Start time; 16 ms aligned for watchdog
static const uint32_t START = UINT32_MAX - 16383UL;

// Loop period
static const uint32_t PERIOD = 640UL;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("Cosa" NAME "Delay: started"));
  trace.flush();

  // Start time and period in milli-seconds
  TRACE(START);
  TRACE(PERIOD);

  // Start timer and set milli-seconds clock
  TIMER::begin();
  TIMER::millis(START);
}

void loop()
{
  static uint16_t cycle = 0;
  uint32_t now = TIMER::millis();
  trace << now
	<< ':' << TIMER::since(START)
	<< ':' << cycle++
	<< endl;
  delay(PERIOD - TIMER::since(now));
}
