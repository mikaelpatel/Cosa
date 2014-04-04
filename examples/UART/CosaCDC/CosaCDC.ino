/**
 * @file CosaCDC.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Test and demonstration of USB/CDC serial interface.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/CDC.hh"

OutputPin ledPin(Board::LED);

void setup()
{
  RTC::begin();
  Watchdog::begin();
  cdc.begin();
  trace.begin(&cdc, PSTR("CosaCDC: started"));
}

void loop()
{
  Watchdog::delay(1000);
  ledPin.toggle();
  trace << Watchdog::millis() << ':' << RTC::millis() << endl;
  Watchdog::delay(128);
  ledPin.toggle();
}

