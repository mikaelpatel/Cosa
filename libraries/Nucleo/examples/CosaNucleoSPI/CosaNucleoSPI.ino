/**
 * @file CosaNucleoSPI.ino
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
 * Demonstrate multi-tasking with SPI access; mutual exclusive access
 * to SPI bus and settings.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>

#include "Cosa/SPI.hh"
#include "Cosa/Math.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"

class Driver : public Nucleo::Thread, public SPI::Driver {
public:
  Driver(Board::DigitalPin pin, SPI::Pulse pulse, SPI::Clock rate, uint8_t mode) :
    Nucleo::Thread(),
    SPI::Driver(pin, pulse, rate, mode)
  {}

  virtual void run()
  {
    // This is an SPI transaction with two blocks. They must complete
    // before the transaction is completed
    spi.acquire(this);
      spi.begin();
      uint8_t buf[] = { 0x55, 0x55, 0x55, 0x55, 0x55 };
      spi.transfer(buf, sizeof(buf));
      spi.end();
      // A yield shall not transfer control of the SPI module
      // The context switch back and forth with give a delay
      yield();
      spi.begin();
      spi.transfer(0xaa);
      spi.end();
      // The logic analyzer should show always two chip selects
      // per driver
    spi.release();
    // Some processing in the driver
    delay(10);
  }
};

// Two thread with different SPI settings
Driver dev1(Board::D2, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0);
Driver dev2(Board::D3, SPI::ACTIVE_HIGH, SPI::DIV8_CLOCK, 3);

void setup()
{
  // Use the watchdog for low-power delay
  Watchdog::begin();

  // Allocate the threads with given stack size
  Nucleo::Thread::begin(&dev1, 64);
  Nucleo::Thread::begin(&dev2, 64);

  // Start the threads
  Nucleo::Thread::begin();
}

void loop()
{
  // Service the nucleos
  Nucleo::Thread::service();
}
