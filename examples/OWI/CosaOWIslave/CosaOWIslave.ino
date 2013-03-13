/**
 * @file CosaOWIslave.ino
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
 * Cosa demonstration of the OWI Master-Slave.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OWI.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// The slave device rom identity (crc is automatically generated)
uint8_t rom[OWI::ROM_MAX] EEMEM = {
  0xC0, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb1
};

// The OWI connection on interrupt pin 0 and led for heartbeat
OWI::Device slave(Board::EXT0, rom);
OutputPin ledPin(Board::LED);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaOWIslave: started"));
  TRACE(free_memory());
  slave.enable();
  Watchdog::begin();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  ledPin.toggle();
  event.dispatch();
  ledPin.toggle();
}
