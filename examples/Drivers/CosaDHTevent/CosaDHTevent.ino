/**
 * @file CosaDHTevent.ino
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
 * Cosa demonstration of the DHT11/DHT22 device driver; event
 * driven mode.
 *
 * @section Circuit
 * Connect Arduino:D2 to DHT11 data pin. Connect D3 to DHT22 data 
 * pin. Pullup resistors (4K7) necessary if long wires. Internal 
 * pullup active. Connect power (VCC) and ground (GND).   
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/DHT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

class Tracer : public DHT22 {
public:
  Tracer(Board::ExternalInterruptPin pin) : DHT22(pin) {}
  virtual void on_sample_completed() { trace << *this << endl; }
};

Tracer dht(Board::EXT0);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHTevent: started"));
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  RTC::begin();
  dht.begin();
}
