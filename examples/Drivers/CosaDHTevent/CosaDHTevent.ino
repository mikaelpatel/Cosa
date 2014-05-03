/**
 * @file CosaDHTevent.ino
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
 * @section Description
 * Cosa demonstration of the DHT11/DHT22 device driver; event
 * driven mode.
 *
 * @section Circuit
 *                       DHT11/dht
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D2/EXT0)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/DHT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

// Example of DHT11 sub-class and periodic member function;
// on_sample_completed() prints current value to the trace stream.
class DHTevent : public DHT11 {
public:
  DHTevent(Board::ExternalInterruptPin pin = Board::EXT0) : DHT11(pin) {}
  virtual void on_sample_completed() { trace << *this << endl; }
};

// The DHT event object
DHTevent dht;

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
