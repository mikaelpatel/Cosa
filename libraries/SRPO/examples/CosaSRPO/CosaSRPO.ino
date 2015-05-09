/**
 * @file CosaSRPO.ino
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
 * Demonstrate Cosa N-Shift Register (SRPO) SPI device driver and Low
 * Power handling. Cascade two shift register (74HC595) and connect
 * LEDs. Circuit below shows ATtinyX5 and 2 X 74HC595 schematics.
 *
 * @section Circuit
 * @code
 *                         ATinyX5
 *                       +----U----+
 * (/RESET)------------1-|PB5   VCC|-8------------(VCC)
 * (D3/A3)--------+----2-|PB3   PB2|-7---------(D2/SCK)----+
 *                |    3-|PB4   PB1|-6--------(D1/MOSI)--+ |
 * (GND)----------(----4-|GND   PB0|-5                   | |
 *                |      +---------+                     | |
 *                +--------------------------------------(-(-+
 *                                                       | | |
 *                         74HC595    (VCC)              | | |
 *                       +----U----+    |                | | |
 * (LCD1/GREEN)--------1-|Q1    VCC|-16-+                | | |
 * (LCD1/BLUE)---------2-|Q2     Q0|-15------(LCD1/RED)  | | |
 *                     3-|Q3    SER|-14---(MOSI/D11/D1)--+ | |
 *                     4-|Q4    /OE|-13-----------(GND)    | |
 *                     5-|Q5   RCLK|-12-----(EN/D10/D3)----(-+
 *                     6-|Q6   SCLK|-11----(SCK/D13/D2)----+ |
 *                     7-|Q7    /MR|-10-----------(VCC)    | |
 *                   +-8-|GND   Q7S|--9------------------+ | |
 *                   |   +---------+                     | | |
 *                   |      0.1uF                        | | |
 *                 (GND)-----||-------(VCC)              | | |
 *                                      |                | | |
 *                         74HC595      |                | | |
 *                       +----U----+    |                | | |
 * (LCD2/GREEN)--------1-|Q1    VCC|-16-+                | | |
 * (LCD2/BLUE)---------2-|Q2     Q0|-15------(LCD2/RED)  | | |
 *                     3-|Q3    SER|-14------------------+ | |
 *                     4-|Q4    /OE|-13-----------(GND)    | |
 *                     5-|Q5   RCLK|-12--------------------(-+
 *                     6-|Q6   SCLK|-11--------------------+
 *                     7-|Q7    /MR|-10-----------(VCC)
 *                   +-8-|GND   Q7S|--9
 *                   |   +---------+
 *                   |      0.1uF
 *                 (GND)-----||-------(VCC)
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <SRPO.h>

#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"

// Power-down delay
#define POWERDOWN_DELAY(ms)				\
  do {							\
    uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);	\
    Power::all_disable();				\
    delay(ms);						\
    Power::all_enable();				\
    Power::set(mode);					\
  } while (0)

// Two cascaded shift registers; use D10/D3 as chip select pin,
// MOSI for serial data and SCK for clock.
SRPO<2> port;

void setup()
{
  // Setup the watchdog; use power-down period as wakeup time
  Watchdog::begin(512);

  // Blink the blue leds; local pin variable
  SRPO<2>::OutputPin blue1(&port, 2);
  SRPO<2>::OutputPin blue2(&port, 10);
  for (uint8_t i = 0; i < 10; i++) {
    blue1.toggle();
    blue2.toggle();
    port.update();
    POWERDOWN_DELAY(512);
  }
}

void loop()
{
  // Cycle through the shift-register parallel output pins (0..15)
  static uint8_t pin = 0;

  // Set pin and update shift register
  port.set(pin);
  port.update();
  POWERDOWN_DELAY(512);

  // Clear pin and update shift register
  port.clear(pin);
  port.update();
  POWERDOWN_DELAY(512);

  // Step to the next pin (0..15)
  pin = (pin + 1) & 0xf;
}
