/**
 * @file CosaBlinkProtoThread.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * The classic LED blink example using a proto-thread function and
 * timer events.
 *
 * @section Circuit
 * No special circuit needed as the built-in LED is used for this example.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <ProtoThread.h>

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

class LED : public ProtoThread {
public:
  LED(Board::DigitalPin pin, Job::Scheduler* scheduler, uint16_t ms) :
    ProtoThread(scheduler),
    m_pin(pin),
    m_delay(ms)
  {}

  virtual void on_run(uint8_t type, uint16_t value)
  {
    UNUSED(type);
    UNUSED(value);
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

Watchdog::Scheduler scheduler;
LED builtin(Board::LED, &scheduler, 512);

void setup()
{
  Watchdog::begin();
  builtin.begin();
}

void loop()
{
  ProtoThread::dispatch();
}
