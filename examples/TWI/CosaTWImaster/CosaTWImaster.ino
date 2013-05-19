/**
 * @file CosaTWImaster.ino
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
 * Cosa demonstration of a TWI master (see also CosaTWIslave).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// TWI slave address
static const uint8_t ADDR = 0xC05A;

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTWImaster: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(TWI));
  
  // Start the watchdog ticks counter
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
   // Write a command to the slave
  static uint32_t cmd = 0;
  static uint32_t start = 0;
  uint32_t delta;
  uint32_t now;
  int count;

  ledPin.toggle();
  twi.begin();
  now = RTC::millis();
  delta = now - start;
  start = now;
  trace << delta << ':';
  TRACE(count = twi.write(ADDR, &cmd, sizeof(cmd)));
  if (count > 0) trace.print(&cmd, count);
  
  // Read back the result
  uint8_t buf[4];
  do {
    now = RTC::millis();
    delta = now - start;
    start = now;
    trace << delta << ':';
    TRACE(count = twi.read(ADDR, buf, sizeof(buf)));
  } while (count < 0);
  twi.end();
  if (count > 0) trace.print(buf, count);
  ledPin.toggle();

  // Next transaction
  cmd += 1;
  SLEEP(2);
}
