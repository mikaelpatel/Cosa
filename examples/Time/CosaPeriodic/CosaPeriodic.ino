/**
 * @file CosaPeriodic.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Demonstrate Periodic functions and verify scheduler time wrap.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class LED : public Periodic, private OutputPin {
public:
  LED(Job::Scheduler* scheduler, uint16_t ms) :
    Periodic(scheduler, ms),
    OutputPin(Board::LED)
  {}
  virtual void run()
  {
    toggle();
  }
};

const uint32_t START = 0xfffff000UL;
Watchdog::Scheduler scheduler;
LED led(&scheduler, 512);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPeriodic: started"));
  trace.flush();

  Watchdog::begin();
  Watchdog::job(&scheduler);
  Watchdog::millis(START);
  led.start();
}

void loop()
{
  Event::service();
  trace << Watchdog::millis() << endl;
}
