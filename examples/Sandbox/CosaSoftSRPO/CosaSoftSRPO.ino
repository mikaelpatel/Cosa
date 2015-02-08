/**
 * @file CosaSoftSRPO.ino
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
 * Demonstrate Cosa Soft N-Shift Register (Soft::SRPO) device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Soft/SRPO.hh"

// Power-down delay
#define POWERDOWN_DELAY(ms)				\
  do {							\
    uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);	\
    Power::all_disable();				\
    delay(ms);						\
    Power::all_enable();				\
    Power::set(mode);					\
  } while (0)

// Soft shift register parallel output port; two cascaded 75HC164
Soft::SRPO<2> port;

void setup()
{
  // Setup the watchdog; use power-down period as wakeup time
  Watchdog::begin();

  // Toggle a led pin
  Soft::SRPO<2>::OutputPin led(&port, 8);
  for (uint8_t i = 0; i < 20; i++) {
    led.toggle();
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
