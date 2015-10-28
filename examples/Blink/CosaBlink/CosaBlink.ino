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
 * 		      Lilypad      Pro-Micro
 * 1. Baseline		9.3	     15.8       mA
 * 2. Disable modules   8.2	     14.5	mA
 * 3. Use watchdog	4.5	      8.1	mA
 * 4. Sleep mode
 *    a. Standby        280	      600	uA
 *    b. Ext Standby	280	      600	uA
 *    c. Powerdown	110	      260	uA
 * 5. Watchdog period	 50	      200	uA
 * 6. Output pin	  6	      146	uA
 * 7. Remove regulator   NA            12	uA
 *
 * Note: The Cosa approach to low power is to provide as much as
 * possible by default, built-in and with additional support functions
 * to reduce power consumption.
 *
 * @section Circuit
 * Uses built-in LED (D13/Arduino). Measurement of mA/uA on VCC from
 * FTDI which is kept connected (See 6). Power LED removed on Pro-Mini.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // 1. Power consumption: 9.3/15.8 mA (Lilypad/Pro-Mini)
  // Busy-wait implementation of delay as Arduino.
  // Power::all_enable();

  // 2. Power Reduction: 8.2/14.5 mA
  // Baseline measurement with disable of all modules (main.cpp).

  // 3. With watchdog delay: 4.5/8.1 mA
  // Start the watchdog with the default timeout period (16 ms)
  // Will install low-power version of the delay() function
  RTT::begin();
  Watchdog::begin();

  // 4. Sleep mode with watchdog delay (3)
  // a. Idle: 7.1/8.1 mA (default)
  // Power::set(SLEEP_MODE_IDLE);
  // b. ADC Noise Reduction: /7.7 mA
  // Power::set(SLEEP_MODE_ADC);
  // c. Standby: 280/600 uA
  // Power::set(SLEEP_MODE_STANDBY);
  // d. Extended standby: 280/600 uA
  // Power::set(SLEEP_MODE_EXT_STANDBY);
  // e. Power-save: /260 uA
  // Power::set(SLEEP_MODE_PWR_SAVE);
  // f. Power-down: 110/260 uA
  Power::set(SLEEP_MODE_PWR_DOWN);

  // 5. Watchdog period change with powerdown (4e)
  // Watchdog shutdown: 50/200 uA
  // #define USE_WATCHDOG_SHUTDOWN

  // 6. FTDI RX/D0 leakage with watchdog shutdown (5)
  // Output mode: 6/146 uA
  // OutputPin::mode(Board::D0);

  // 7. Remove Pro-Micro regulator
}

void loop()
{
  // Turn on the led
  ledPin.on();

  // 5a. Turn on watchdog: 16 ms timeout
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::begin(16);
#endif

  delay(1);

  // 5b. Turn off watchdog
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::end();
#endif

  // Turn off the led
  ledPin.off();

  // 5c. Turn on watchdog: 512 ms timeout
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::begin(512);
#endif

  delay(2000);

  // 5d. Turn off watchdog
#ifdef USE_WATCHDOG_SHUTDOWN
  Watchdog::end();
#endif
}
