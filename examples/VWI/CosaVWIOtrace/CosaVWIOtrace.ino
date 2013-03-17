/**
 * @file CosaVWIOtrace.ino
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
 * Demonstration of the Virtual Wire Interface Output Stream
 * driver. 
 *
 * @section Circuit
 * Connect RF433/315 Transmitter to Arduino D12.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/VWIO.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"

VirtualWireCodec codec;
VWIO tx(Board::D12, &codec);

void setup()
{
  // Start watchdog and real-time clock
  Watchdog::begin();
  RTC::begin();

  // Start virtual wire output stream and trace
  tx.begin(4000);
  trace.begin(&tx, PSTR("CosaVWIOtrace: started"));

  // Trace some memory information
  TRACE(free_memory());
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(VWI::Transmitter));
  TRACE(sizeof(VWIO));
}

void loop()
{
  // Monitor digital pin values
  trace << RTC::millis() << PSTR(": D0..10:");
  for (uint8_t i = 0; i < 11; i++)
    trace << ' ' << InputPin::read(i);
  trace << endl;
  SLEEP(2);

  // Monitor analog pin values
  trace << RTC::millis() << PSTR(": A0..7:");
  for (uint8_t i = 0; i < 8; i++)
    trace << ' ' << AnalogPin::sample(i);
  trace << endl;
  SLEEP(2);
}
