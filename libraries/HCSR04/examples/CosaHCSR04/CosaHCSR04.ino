/**
 * @file CosaHCSR04.ino
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
 * Cosa demonstration of the Ultrasonic range module HC-SR04 driver.
 * Periodic event driven object with callback member function.
 *
 * @section Circuit
 * @code
 *                           HC-SR04
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (D2)----------------2-|TRIG        |
 * (D3) ---------------3-|ECHO        |
 * (GND)---------------4-|GND         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <HCSR04.h>

#include "Cosa/Job.hh"
#include "Cosa/Event.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

class Ping : public HCSR04 {
public:
  Ping(Job::Scheduler* scheduler, Board::DigitalPin trigger, Board::DigitalPin echo) :
    HCSR04(scheduler, trigger, echo)
  {}
  virtual void on_change(uint16_t distance)
  {
    TRACE(distance);
  }
};

Watchdog::Scheduler scheduler;
Ping ping(&scheduler, Board::D2, Board::D3);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaHCSR04: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(HCSR04));

  // Start the watchdog ticks
  Watchdog::begin();

  // Schedule to read distance every 1/4 second
  ping.schedule(256);
}

void loop()
{
  Event::service();
}
