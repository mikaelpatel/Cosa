/**
 * @file CosaHMC5883L.ino
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
 * Cosa HMC5883L 3-Axis Digital Compass IC driver example.
 *
 * @section Circuit
 * The GY-80 10DOF module with pull-up resistors (4K7) for TWI signals and
 * 3V3 internal voltage converter.
 * @code
 *                           GY-80
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 *                     2-|3V3         |
 * (GND)---------------3-|GND         |
 * (A5/SCL)------------4-|SCL         |
 * (A4/SDA)------------5-|SDA         |
 *                     6-|M-DRDY      |
 *                     7-|A-INT1      |
 *                     8-|T-INT1      |
 *                     9-|P-XCLR      |
 *                    10-|P-EOC       |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <HMC5883L.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// The 3-Axis Digital Compass
HMC5883L compass;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaHMC5883L: started"));

  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(HMC5883L));

  // Set continous measurement mode, 3 Hz output, avg 8 samples, +-4.0 Gauss
  compass.output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.range(HMC5883L::RANGE_4_0_GA);
  compass.mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);

  // Start the watchdog ticks and the compass
  Watchdog::begin();
  TRACE(compass.begin());
}

void loop()
{
  // Read the heading, scale to milli gauss and print the data
  compass.read_heading();
  compass.to_milli_gauss();
  trace << compass << endl;
  sleep(2);
}
