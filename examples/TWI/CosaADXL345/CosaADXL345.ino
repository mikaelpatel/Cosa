/**
 * @file CosaAXDL345.ino
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
 * Cosa demonstration of ADXL345 driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/ADXL345.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
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
