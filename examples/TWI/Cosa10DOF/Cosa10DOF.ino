/**
 * @file Cosa10DOF.ino
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
 * Cosa demonstration of 10 DOF module (GY-80) with ADXL345, BMP085,
 * HMC5883L, and L3G4200D; 3-axis acceleratometer, thermometer, barometer,
 * 3-axis compass and 3-axis gyroscope.
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

#include <ADXL345.h>
#include <BMP085.h>
#include <HMC5883L.h>
#include <L3G4200D.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Digital acceleratometer with alternative address
ADXL345 acceleratometer(1);

// Digital temperature and pressure sensor
BMP085 bmp;

// The 3-Axis Digital Compass
HMC5883L compass;

// Digital Gyroscope using alternative address
L3G4200D gyroscope(1);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("Cosa10DOF: started"));

  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(ADXL345));
  TRACE(sizeof(BMP085));
  TRACE(sizeof(HMC5883L));
  TRACE(sizeof(L3G4200D));

  // Start the watchdog ticks and the sensors
  Watchdog::begin();
  TRACE(acceleratometer.begin());
  TRACE(bmp.begin(BMP085::ULTRA_LOW_POWER));
  TRACE(gyroscope.begin());

  // Set continous measurement mode, 3 Hz output, avg 8 samples, +-4.0 Gauss
  compass.output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.range(HMC5883L::RANGE_4_0_GA);
  compass.mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);
  TRACE(compass.begin());

  sleep(1);
}

void loop()
{
  // Sample sensor and print temperature and pressure
  bmp.sample();
  trace << bmp << endl;

  // Read the heading, scale to milli gauss and print the data
  compass.read_heading();
  compass.to_milli_gauss();
  trace << compass << endl;

  // Sample and print measurement to output stream
  trace << acceleratometer << endl;

  // Periodically sample the printout the gyroscope reading
  trace << gyroscope << endl;
  trace << endl;
  sleep(2);
}
