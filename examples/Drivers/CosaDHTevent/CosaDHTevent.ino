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
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

// DHT22 data pin configuration
#if defined(__ARDUINO_TINY__)
#define EXT Board::EXT0
#else
#define EXT Board::EXT1
#endif

// Example of DHT22 sub-class and periodic member function;
// on_sample_completed() prints current value to the trace stream.
class DHTevent : public DHT22 {
public:
  DHTevent(Board::ExternalInterruptPin pin) : DHT22(pin) {}
  virtual void on_sample_completed() { trace << *this << endl; }
};

// The DHT event object
DHTevent dht(EXT);

void setup()
{
  // The trace output stream on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHTevent: started"));

  // Watchdog will issue timeout events
  Watchdog::begin(16, Watchdog::push_timeout_events);
  RTC::begin();

  // The DHT event object is started with default 2 second period
  dht.begin();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
