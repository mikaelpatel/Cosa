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

/**
 * SPI device driver steps:
 *
 * spi.acquire(&driver);
 *   Acquire the SPI device driver.
 *   Disable all other SPI device drivers.
 *   Set up SPI control registers.
 *
 * spi.begin();
 *   Assert chip select if active low/high.
 *
 * spi.transfer();
 *   Transfer data.
 *
 * spi.end();
 *   Deassert chip select or pulse.
 *
 * spi.release();
 *   Release the SPI device driver.
 *   Enable all SPI device drives.
 *
 * The debug pin marks the acquire-release block.
 */

SPI::Driver dev1(Board::D2, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0);
SPI::Driver dev2(Board::D3, SPI::ACTIVE_HIGH, SPI::DIV8_CLOCK, 1);
SPI::Driver dev3(Board::D4, SPI::PULSE_LOW, SPI::DIV16_CLOCK, 2);
SPI::Driver dev4(Board::D5, SPI::PULSE_HIGH, SPI::DIV32_CLOCK, 3);

OutputPin debug(Board::D6, 1);

void loop()
{
  // Sleep to synch with logic analyser
  sleep(5);

  // Single byte transfer (dev1@8MHz)
  // debug=10.5, cs=2.5 (low), diff=8.0
  // 100 Kbyte/s (max 1 Mbyte/s)
  debug.clear();
  spi.acquire(&dev1);
    spi.begin();
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();

  // 8 byte block transfer (dev1@8MHz)
  // debug=21.12 cs=13.0 (low), diff=8.12
  // 592 Kbyte/s (max 1 Mbyte/s)
  const size_t BUF_MAX = 8;
  uint8_t buf[BUF_MAX];
  debug.clear();
  spi.acquire(&dev1);
    spi.begin();
      spi.write(buf, sizeof(buf));
    spi.end();
  spi.release();
  debug.set();

  // Double byte transfer (dev1@8MHz)
  // debug=12.12, cs=4.0 (low), diff=8.12
  // 413 Kbyte/s (max 500 Kbyte/s)
  debug.clear();
  spi.acquire(&dev1);
    spi.begin();
      spi.transfer(0x55);
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();

  // Double byte transfer (dev2@2MHz)
  // debug=18.1, cs=10.0 (high)
  // 276 Kbyte/s (max 500 Kbyte/s)
  debug.clear();
  spi.acquire(&dev2);
    spi.begin();
      spi.transfer(0x55);
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();

  // Double byte transfer (dev3@1MHz)
  // debug=26.1, cs=1.2 (pulse low)
  // 192 Kbyte/s (max 250 Kbyte/s)
  debug.clear();
  spi.acquire(&dev3);
    spi.begin();
      spi.transfer(0x55);
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();

  // Double byte tranfer (dev4@500KHz)
  // debug=42.1, cs=1.2 (pulse high)
  // 119 Kbyte/s (max 125 Kbyte/s)
  debug.clear();
  spi.acquire(&dev4);
    spi.begin();
      spi.transfer(0x55);
      spi.transfer(0x55);
    spi.end();
  spi.release();
  debug.set();
}
