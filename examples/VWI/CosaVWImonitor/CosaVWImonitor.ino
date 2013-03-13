/**
 * @file CosaVWImonitor.ino
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
 * @section Description
 * Demonstration of the Virtual Wire Interface (VWI) driver.
 * Print incoming messages as a character stream.
 *
 * @section Circuit
 * Connect RF433/315 Receiver to Arduino D11.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

VirtualWireCodec codec;
VWI::Receiver rx(Board::D11, &codec);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVWImonitor: started"));
  TRACE(free_memory());
  VWI::begin(4000);
  rx.begin();
}

void loop()
{
  rx.await();
  char buffer[VWI::PAYLOAD_MAX];
  int8_t len = rx.recv(buffer, sizeof(buffer));
  for (uint8_t i = 0; i < len; i++)
    trace.print(buffer[i]);
}
