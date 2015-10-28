/**
 * @file CosaTWImaster.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * @section Circuit
 * The Arduino analog pins 4 (SDA) and 5 (SCL) are used for I2C/TWI
 * connection.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// TWI slave address
static const uint8_t ADDR = 0x5A;
TWI::Driver dev(ADDR);

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
  RTT::begin();
}

void loop()
{
   // Write a command to the slave
  static uint16_t cmd = 0;
  uint32_t time;
  int count;

  ledPin.toggle();
  time = RTT::micros();
  twi.acquire(&dev);
  count = twi.write(&cmd, sizeof(cmd));
  twi.release();
  time = RTT::micros() - time;
  trace << PSTR("write(2):") << time << ':' << time/sizeof(cmd) << ':';
  if (count > 0) trace.print(&cmd, count);

  // Read back the result
  uint8_t buf[8];

  time = RTT::micros();
  twi.acquire(&dev);
  do {
    count = twi.read(buf, sizeof(buf));
  } while (count < 0);
  twi.release();
  time = RTT::micros() - time;
  trace << PSTR("read(8):") << time << ':' << time/sizeof(buf) << ':';
  if (count > 0) trace.print(buf, count);

  time  = RTT::micros();
  twi.acquire(&dev);
  do {
    count = twi.read(buf, sizeof(buf) - 4);
  } while (count < 0);
  twi.release();
  time = RTT::micros() - time;
  trace << PSTR("read(4):") << time << ':' << time/(sizeof(buf) - 4) << ':';
  if (count > 0) trace.print(buf, count);
  ledPin.toggle();
  trace << endl;

  // Next transaction
  cmd += 1;
  sleep(2);
}
