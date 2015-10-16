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
 * delay.
 *
 * @section Circuit
 * Uses built-in LED (D13/Arduino). Measurement of mA/uA on VCC from
 * FTDI. Power LED removed on Pro-Mini.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // 1. Power consumption: 9.9/15.8 mA (Lilypad/Pro-Micro)
  // Busy-wait implementation of delay as Arduino.
  // Power::all_enable();

  // 2. Power Reduction: 10.5/14.5 mA
  // Baseline measurement with disable of all modules (main.cpp).

  // 3. With watchdog delay: 7.1/8.1 mA
  // Start the watchdog with the default timeout period (16 ms)
  // Will install low-power version of the delay() function
  // Watchdog::begin();

  // 4. Sleep mode with watchdog delay (3)
  // a. Idle: 7.1/8.1 mA (default)
  // Power::set(SLEEP_MODE_IDLE);
  // b. Standby: 280/600 uA
  // Power::set(SLEEP_MODE_STANDBY);
  // c. Extended standby: 280/600 uA
  // Power::set(SLEEP_MODE_EXT_STANDBY);
  // d. Power-down: 110/260 uA
  Power::set(SLEEP_MODE_PWR_DOWN);

  // 5. Watchdog period change with powerdown (4d)
  // Watchdog shutdown: 50/200 uA
  #define USE_WATCHDOG_SHUTDOWN

  // 6. FTDI RX/D0 leakage with watchdog shutdown (5)
  // Output mode: 6/146 uA
  OutputPin::set_mode(Board::D0);

  // 7. Remove Pro-Micro reglator
  // This will remove the difference.

  // NB: The Cosa approach to low power is to provide
  // as much as possible built-in and with additional
  // support functions to reduce power consumption.
}

void loop()
{
  // Turn on the led
  ledPin.on();

  // 5a. Turn on watchdog: 16 ms timeout
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::begin(16);
#endif

  delay(10);

  // 5b. Turn off watchdog
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::end();
#endif

  // Turn off the led
  ledPin.off();

  // 5c. Turn on watchdog: 1024 ms timeout
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::begin(1024);
#endif

  delay(2000);

  // 5d. Turn off watchdog
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::end();
#endif
}
