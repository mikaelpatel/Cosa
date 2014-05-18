/**
 * @file CosaSRPO.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * Demonstrate Cosa N-Shift Register (SRPO) SPI class. Cascade two
 * shift register (74HC595) and connect LEDs. See SRPO.hh for more
 * details on the circuit.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI/Driver/SRPO.hh"
#include "Cosa/Watchdog.hh"

// Two cascaded shift registers; use D10/D3 as chip select pin, 
// MOSI for serial data and SCK for clock. 
SRPO<2> port;

void setup()
{
  Watchdog::begin();
}

void loop()
{
  static uint8_t pin = 0;

  // Set pin and update shift register
  port.set(pin);
  port.update();
  delay(500);

  // Clear pin and update shift register
  port.clear(pin);
  port.update();
  delay(500);

  // Step to the next pin (0..15)
  pin = (pin + 1) & 0xf;
}
