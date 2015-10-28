/**
 * @file CosaAXDL345.ino
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
 * Cosa demonstration of ADXL345 driver.
 *
 * @section Circuit
 * The GY-291 module with pull-up resistors (4K7) for TWI signals and
 * 3V3 internal voltage converter.
 * @code
 *                           GY-291
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (VCC)---------------2-|VCC         |
 *                     3-|CS          |
 *                     4-|A-INT1      |
 *                     5-|A-INT2      |
 *                     6-|SDO         |
 * (A4/SDA)------------7-|SDA         |
 * (A5/SCL)------------8-|SCL         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <ADXL345.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Digital accelerometer with alternative address
ADXL345 accelerometer(1);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaADXL345: started"));

  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(ADXL345));

  // Start the watchdog ticks and the accelerometer
  Watchdog::begin();
  TRACE(accelerometer.begin());
}

void loop()
{
  // Check for activity on the accelerometer. Print value and type
  uint8_t source = accelerometer.is_activity();
  if (source != 0) {
    trace << Watchdog::millis() << ':' << accelerometer;
    if (source & _BV(ADXL345::FREE_FALL)) trace << PSTR(", free fall");
    if (source & _BV(ADXL345::INACT)) trace << PSTR(", inactivity");
    if (source & _BV(ADXL345::ACT)) trace << PSTR(", activity");
    if (source & _BV(ADXL345::DOUBLE_TAP)) trace << PSTR(", double tap");
    if (source & _BV(ADXL345::SINGLE_TAP)) trace << PSTR(", single tap");
    trace << endl;
  }
  Watchdog::delay(64);
}
