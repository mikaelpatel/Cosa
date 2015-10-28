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
 * Test RTC and Watchdog delay wrap around behavior and low power
 * sleep modes.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Configuration
#define USE_RTT
// #define USE_WATCHDOG

// Use the RTT based delay. Timer will continue to update during the delay.
#if defined(USE_RTT)
#include "Cosa/RTT.hh"
#define TIMER RTT
#define NAME "RTT"
#define SLEEP_MODE SLEEP_MODE_EXT_STANDBY
#endif

// Use the Watchdog based delay.
#if defined(USE_WATCHDOG)
#include "Cosa/Watchdog.hh"
#define TIMER Watchdog
#define NAME "Watchdog"
#define SLEEP_MODE SLEEP_MODE_PWR_DOWN
#endif

// Start time to force wrap-around; 16 ms aligned for watchdog
static const uint32_t START = UINT32_MAX - 16383UL;

// Loop period
static const uint32_t PERIOD = 1280UL;

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
  trace << cycle++
	<< ':' << now
	<< ':' << TIMER::since(START)
	<< endl;
  trace.flush();

  // Use defined sleep mode during delay to power down
  // Baseline: 8.3 mA (Pro-Mini with power LED removed)
  // RTT:      1.5 mA
  // Watchdog: 260 uA
  // Baseline: 33 mA (Mega 2560, Vin 5V)
  // RTT:      16 mA
  // Watchdog: 14 mA
  uint8_t mode = Power::set(SLEEP_MODE);
  delay(PERIOD - TIMER::since(now));
  Power::set(mode);
}
