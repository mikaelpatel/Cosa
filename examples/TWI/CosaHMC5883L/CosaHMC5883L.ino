/**
 * @file CosaHMC5883L.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa HMC5883L 3-Axis Digital Compass IC driver example.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/HMC5883L.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
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

  // Start the watchdog ticks and the compass
  Watchdog::begin();
  compass.begin();

  // Read the first sample
  compass.read_heading();
  trace << compass << endl;
  compass.end();

  // Set continous measurement mode, 3 Hz output, avg 8 samples, +-4.0 Gauss
  compass.set_output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.set_samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.set_range(HMC5883L::RANGE_4_0_GA);
  compass.set_mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);
  compass.begin();
}

void loop()
{
  // Read the heading and print the raw data
  compass.read_heading();
  trace << compass;

  // Scale to milli gauss and print the data
  compass.to_milli_gauss();
  trace << compass;
  trace << endl;
  SLEEP(2);
}
