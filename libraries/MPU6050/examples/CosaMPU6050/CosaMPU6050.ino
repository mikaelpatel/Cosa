/**
 * @file CosaMPU6050.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Cosa demonstration of MPU6050 driver.
 *
 * @section Circuit
 * The MPU6050 module ITG/MPU with pull-up resistors (4K7) for TWI
 * signals and 3V3 internal voltage converter.
 * @code
 *                           ITG/MPU
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (A5/SCL)------------3-|SCL         |
 * (A4/SDA)------------4-|SDA         |
 *                     6-|XDA         |
 *                     7-|XCL         |
 *                     8-|AD0         |
 *                     9-|INT         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <MPU6050.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Digital mpu with alternative address
MPU6050 mpu;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMPU6050: started"));

  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(MPU6050));

  // Start the watchdog ticks and the mpu
  Watchdog::begin();
  TRACE(mpu.begin());
}

void loop()
{
  // Sample and print measurement to output stream
  trace << mpu;
  sleep(2);
}
