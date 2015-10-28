/**
 * @file CosaCDC.ino
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
 * Test and demonstration of USB/CDC serial interface.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/CDC.hh"

#if !defined(USBCON)
#error CosaCDC: board not supported.
#endif

OutputPin ledPin(Board::LED);

void setup()
{
  RTT::begin();
  Watchdog::begin();
  cdc.begin();
  trace.begin(&cdc, PSTR("CosaCDC: started"));
}

void loop()
{
  Watchdog::delay(1000);
  ledPin.toggle();
  trace << Watchdog::millis() << ':' << RTT::millis() << endl;
  Watchdog::delay(128);
  ledPin.toggle();
}

