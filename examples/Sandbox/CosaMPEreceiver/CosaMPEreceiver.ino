/**
 * @file CosaMPEreceiver.ino
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
 * Demonstraction of Cosa MPE (Manchester Phase Encoding); 
 * Connect RF433 receiver module to Board::PCI8 (D8).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/MPE.hh"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

MPE::Receiver rx;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMPEreceiver: started"));
  Watchdog::begin();
  RTC::begin();
  PinChangeInterrupt::begin();
  rx.begin();
}

void loop()
{
  static uint8_t nr = 0;
  static uint16_t count = 0;
  static uint16_t timeouts = 0;
  static uint16_t errors = 0;
  static uint16_t drops = 0;

  uint8_t buf[MPE::PAYLOAD_MAX];
  int n = rx.recv(buf, sizeof(buf), 1000);
  if (n > 0) {
    count += 1;
    if (buf[0] != nr) {
      drops += 1;
      errors += 1;
      nr = buf[0];
    }
    nr += 1;
    if (count % 100) return;

    uint32_t pps = count * 1000L / RTC::millis();
    trace << PSTR("Packages received:") << count
	  << endl
	  << PSTR("Packages per second:") << pps 
	  << PSTR(" (") << n << PSTR(" bytes)") 
	  << endl
	  << PSTR("Bytes per second:") << pps * n 
	  << endl
	  << PSTR("MPE bits per second:") << 16 * pps * n 
	  << endl
	  << PSTR("Errors:") << errors
	  << PSTR(" (") << (100L * errors) / (count + errors) 
	  << PSTR("%)") 
	  << endl
	  << PSTR("Timeouts:") << timeouts 
	  << endl
	  << PSTR("Drops:") << drops 
	  << endl;
    trace.print(buf, n);
    trace << endl;
  } else if (n == 0) 
    timeouts += 1;
  else 
    errors += 1;
}

