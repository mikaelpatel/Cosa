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
 * Cosa demonstration of a TWI master (see CosaTWIslave).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/TWI.h"
#include "Cosa/Pins.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Trace.h"

// TWI slave address
static const uint8_t ADDR = 0xC05A;

// The TWI interface
TWI twi;

// Use the builtin led for a heartbeat
OutputPin ledPin(13);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaTWImaster: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog ticks counter
  Watchdog::begin();
}

void loop()
{
  Watchdog::delay(1024);

  // Write a command to the slave
  static uint8_t cmd = 0;
  ledPin.toggle();
  INFO("cmd = %d", cmd);
  twi.begin();
  twi.write(ADDR, cmd++);
  twi.end();

  // Read back the result
  uint8_t buf[4];
  twi.begin();
  int count = twi.read(ADDR, buf, sizeof(buf));
  twi.end();
  if (count > 0) 
    trace.print(buf, count);
  else
    TRACE(count);
  ledPin.toggle();
}
