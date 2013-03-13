/**
 * @file CosaBlinkThread.ino
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
 * @section Description
 * Cosa LED blink with thread function.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Thread.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"

class LED : public Thread {
private:
  OutputPin m_pin;
  uint16_t m_delay;

public:
  LED(Board::DigitalPin pin, uint16_t ms, uint8_t initial = 0) : 
    Thread(), 
    m_pin(pin, initial),
    m_delay(ms)
  {}

  virtual void run(uint8_t type, uint16_t value) 
  { 
    THREAD_BEGIN();
    while (1) {
      m_pin.toggle(); 
      THREAD_DELAY(m_delay);
    }
    THREAD_END();
  }
};

LED builtin(Board::LED, 512);

void setup()
{
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  builtin.begin();
}

void loop()
{
  Thread::dispatch(1);
}
