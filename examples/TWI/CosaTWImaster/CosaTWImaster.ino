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
 * @section Description
 * Cosa demonstration of a TWI master (see also CosaTWIslave).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
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
}

void loop()
{
  Watchdog::delay(512);

  // Write a command to the slave
  static uint8_t cmd = 0;
  uint8_t buf[4];
  ledPin.toggle();
  buf[0] = cmd++;
  for (uint8_t i = 1; i < sizeof(buf); i++) buf[i] = 0;
  INFO("WRITE(cmd = %d)", cmd);
  trace.print(buf, sizeof(buf));
  twi.begin();
  twi.write(ADDR, buf, sizeof(buf));
  twi.end();

  // Read back the result
  twi.begin();
  int count = twi.read(ADDR, buf, sizeof(buf));
  twi.end();
  INFO("READ(count = %d)", count);
  if (count > 0) 
    trace.print(buf, count);
  ledPin.toggle();
}
