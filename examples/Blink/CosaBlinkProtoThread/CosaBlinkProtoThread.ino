/**
 * @file CosaBlinkProtoThread.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * The classic LED blink example using a proto-thread function and 
 * timer events. 
 * 
 * @section Circuit
 * No special circuit needed as the built-in LED is used for this example.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/ProtoThread.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

class LED : public ProtoThread {
public:
  LED(Board::DigitalPin pin, uint16_t ms) : 
    ProtoThread(), 
    m_pin(pin),
    m_delay(ms)
  {}

  virtual void run(uint8_t type, uint16_t value) 
  { 
    PROTO_THREAD_BEGIN();
    while (1) {
      m_pin.toggle(); 
      PROTO_THREAD_DELAY(m_delay);
    }
    PROTO_THREAD_END();
  }

private:
  OutputPin m_pin;
  uint16_t m_delay;
};

LED builtin(Board::LED, 512);

void setup()
{
  Watchdog::begin(16, Watchdog::push_timeout_events);
  builtin.begin();
}

void loop()
{
  ProtoThread::dispatch();
}
