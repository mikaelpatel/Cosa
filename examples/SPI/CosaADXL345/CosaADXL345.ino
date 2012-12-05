/**
 * @file CosaAXDL345.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Cosa demonstration of ADXL345 driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI/ADXL345.h"
#include "Cosa/Pins.h"
#include "Cosa/Trace.h"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.h"

// Digital Accelerometer using SPI and default slave select pin(10)
ADXL345 adxl;

void setup()
{
  // Initiate trace stream
  trace.begin(9600, PSTR("CosaADXL345: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Recalibrate the accelerometer
  Watchdog::delay(100);
  adxl.calibrate();
}

void loop()
{
  // Wait for 500 ms.
  Watchdog::delay(500);

  // Sample the accelerometer and print values to trace stream
  ADXL345::sample_t values;
  adxl.sample(values);
  values.println();
}
