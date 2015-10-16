/**
 * @file CosaBlink.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Cosa LED blink demonstration. The classical LED blink program
 * written in Cosa using the Arduino built-in LED with a very short
 * blink period(10:2000 ms) and the Watchdog for low power mode during
 * delay. Reduce power with clock prescaling and sleep mode setting.
 *
 * @section Circuit
 * Uses built-in LED (D13/Arduino). Measurement of milli-amperes
 * on FTDI VCC.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/CPU.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // 1. Power consumption: 9.9/15.8 mA (Lilypad/Pro-Micro)
  // Busy-wait implementation of delay as Arduino.

  // 2. Power Reduction: 9.0/14.1 mA
  // Baseline measurement with disable of all modules (main.cpp).

  // 3. Frequency scaling:
  // 4/8 Mhz, 7.5/10.8 mA
  // CPU::clock_prescale(1);
  // 2/4 Mhz, 6.2/9.3 mA
  // CPU::clock_prescale(2);
  // 1/2 Mhz, 5.6/8.1 mA
  // CPU::clock_prescale(3);

  // 4. With watchdog delay: 5.6/8.1 mA
  // Start the watchdog with the default timeout period (16 ms)
  // Will install low-power version of the delay() function
  // Watchdog::begin();

  // 5. Sleep mode with watchdog delay
  // Idle: 5.6/8.1 mA (default)
  // Power::set(SLEEP_MODE_IDLE);
  // Standby: 0.3/0.7 mA
  // Power::set(SLEEP_MODE_STANDBY);
  // Extended standby: 0.3/0.7 mA
  // Power::set(SLEEP_MODE_EXT_STANDBY);
  // Power-down: 0.1/0.3 mA
  Power::set(SLEEP_MODE_PWR_DOWN);

  // 6. Watchdog period change with powerdown
  // Watchdog shutdown: 0.06/0.2 mA
  #define USE_WATCHDOG_SHUTDOWN

  // 7. Add RTC (but use watchdog delay)
  // Idle: 8.5 mA
  // Standby: 0.7 mA
  // Ext. standby: 0.7 mA
  // RTC::begin();
  // Watchdog::begin();
}

void loop()
{
  // Turn on the led
  ledPin.on();

  // 6a. Turn on watchdog: 16 ms timeout
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::begin(16);
#endif

  delay(10);

  // 6b. Turn off watchdog
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::end();
#endif

  // Turn off the led
  ledPin.off();

  // 6c. Turn on watchdog: 2048 ms timeout
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::begin(2048);
#endif

  delay(2000);

  // 6d. Turn off watchdog
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::end();
#endif
}
