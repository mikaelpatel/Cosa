/**
 * @file CosaBlinkTemp.ino
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
 * Cosa LED blink demonstation. Blink RGB LED according to temperature
 * from 1-wire digital thermometer (DS18B20); RED if above 30 C,
 * BLUE if below 25 C otherwise GREEN. Demonstrates powerdown to
 * less than 10 uA while idle (and no LED on).
 *
 *                       DS18B20/sensor
 * (D3)---[4K7]--+       +------------+
 * (GND)---------)-----1-|GND         |\
 * (D4)----------+-----2-|DQ          | |
 * (VCC/GND)-----------3-|VDD         |/
 *                       +------------+
 *
 * (D5/D0)------RED->|---+
 *                       |
 * (D6/D1)----GREEN->|---+----[330]---(GND)
 *                       |
 * (D7/D2)-----BLUE->|---+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <OWI.h>
#include <DS18B20.h>

#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"

// RGB LED pins
#if !defined(BOARD_ATTINY)
OutputPin red(Board::D5);
OutputPin green(Board::D6);
OutputPin blue(Board::D7);
#else
OutputPin red(Board::D0);
OutputPin green(Board::D1);
OutputPin blue(Board::D2);
#endif

// 1-wire bus and digital thermometer
OutputPin pw(Board::D3);
OWI owi(Board::D4);
DS18B20 sensor(&owi);

// Powerdown on delay
void powerdown_delay(uint32_t ms)
{
  uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);
  Power::all_disable();
  Watchdog::delay(ms);
  Power::all_enable();
  Power::set(mode);
}

void setup()
{
  // Start watchdog
  Watchdog::begin();
  delay = powerdown_delay;

  // Connect to temperature sensor, and set resolution and triggers
  pw.on();
  sensor.connect(0);
  sensor.resolution(10);
  sensor.set_trigger(25,30);
  sensor.write_scratchpad();
  pw.off();
}

void loop()
{
  // Temperature conversion and read result
  pw.on();
  sensor.convert_request();
  sensor.read_scratchpad();
  pw.off();
  int8_t low, high;
  int16_t temp = (sensor.temperature() >> 4);
  sensor.get_trigger(low, high);

  // Set LED according to temperature
  if (temp < low) {
    blue.on();
    delay(16);
    blue.off();
    delay(1000);
  }
  else if (temp > high) {
    red.on();
    delay(16);
    red.off();
    delay(1000);
  }
  else {
    green.on();
    delay(32);
    green.off();
    delay(2000);
  }
}
