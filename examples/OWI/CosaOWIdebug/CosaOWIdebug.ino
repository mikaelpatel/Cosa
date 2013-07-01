/**
 * @file CosaOWIdebug.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * List connected 1-Wire devices (bus on pin(D7)).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#include "Cosa/OWI.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// One-wire pin
#if defined(__ARDUINO_TINY__)
OWI owi(Board::D1);
#else
OWI owi(Board::D7);
#endif

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaOWIdebug: started"));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // List connected devices
  trace << owi << endl;
}

void loop()
{
  // Search for alarms
  ledPin.toggle();
  OWI::Driver dev(&owi);
  int8_t last = OWI::Driver::FIRST;
  do {
    last = dev.alarm_search(last);
    if (last == OWI::Driver::ERROR) break;
    trace << dev << endl;
  } while (last != OWI::Driver::LAST);
  trace << endl;
  ledPin.toggle();
  
  // Sleep awhile before checking again
  SLEEP(2);
}
