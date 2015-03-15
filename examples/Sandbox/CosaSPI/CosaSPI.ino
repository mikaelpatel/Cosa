/**
 * @file CosaSPI.ino
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
 * Demonstration and verification of SPI timing; clock, mode and
 * chip select handling.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI.hh"
#include "Cosa/OutputPin.hh"

SPI::Driver dev1(Board::D2, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0);
SPI::Driver dev2(Board::D3, SPI::ACTIVE_HIGH, SPI::DIV8_CLOCK, 1);
SPI::Driver dev3(Board::D4, SPI::PULSE_LOW, SPI::DIV32_CLOCK, 2);

OutputPin debug(Board::D6, 1);

/*
 * SPI device driver steps:
 *
 * spi.acquire(&driver);
 *   Acquire the SPI device driver.
 *   Disable all other SPI device drivers.
 *   Set up SPI control registers.
 *
 * spi.begin();
 *   Assert chip select.
 *
 * spi.transfer();
 *   Transfer data.
 *
 * spi.end();
 *   Deassert chip select.
 *
 * spi.release();
 *   Release the SPI device driver.
 *   Enable all SPI device drives.
 *
 * The debug pin marks the acquire-release block.
 */
void loop()
{
  // Sleep to synch with logic analyser
  sleep(5);

  // Single byte transfer (dev1@8MHz)
  // debug=10.5, cs=2.5 (low)
  // 100 Kbyte/s (max 1 Mbyte/s)
  debug.clear();
  spi.acquire(&dev1);
    spi.begin();
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();

  // 10 byte transfer (dev1@8MHz)
  // debug=24.5, cs=16.5 (low)
  // 408 Kbyte/s (max 1 Mbyte/s)
  debug.clear();
  spi.acquire(&dev1);
    spi.begin();
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();

  // Double byte transfer (dev2@1MHz)
  // debug=18.1, cs=10.0 (high)
  // 110 Kbyte/s (max 125 Kbyte/s)
  debug.clear();
  spi.acquire(&dev2);
    spi.begin();
      spi.transfer(0x55);
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();

  // Multiple transfer; single followed by double (dev3@500KHz)
  // debug=61.3, cs=1.1 (pulse)
  // 49 Kbyte/s (max 67 Kbyte/s)
  debug.clear();
  spi.acquire(&dev3);
    spi.begin();
      spi.transfer(0x55);
    spi.end();
    spi.begin();
      spi.transfer(0x55);
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();
}
