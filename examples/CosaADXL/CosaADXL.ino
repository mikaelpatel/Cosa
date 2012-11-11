/**
 * @file
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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

#include "Cosa/Watchdog.h"
#include "Cosa/ADXL.h"

// Digital Accelerometer using SPI and default slave select pin(10)

ADXL adxl;

void setup()
{
  // Start the serial interface 
  Serial.begin(9600);

  // Start the watchdog and push timeout events
  Watchdog::begin(1024, Watchdog::push_event);

  // Calibrate the accelerometer
  adxl.calibrate();
}

void loop()
{
  // Wait for the next event (1 second sleep)
  Event event;
  Event::queue.await(&event);

  // Sample the accelerometer and print values
  ADXL::sample_t s;
  adxl.sample(s);
  s.println();
}
