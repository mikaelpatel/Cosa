/**
 * @file CosaAnalogComparator.ino
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
 * Demonstration of Analog Comparator Interrupt handler.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogComparator.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Event.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

IOStream& operator<<(IOStream& outs, Event& event)
{
  outs << Watchdog::millis() << PSTR(":Event(")
       << event.type() << ','
       << event.target() << ','
       << event.value() << ')';
  return (outs);
}

// Ref. voltage: AIN1 (D7)
// AnalogComparator detector(AnalogComparator::ON_FALLING_MODE);

// Ref. voltage: Bandgap (1V1)
AnalogComparator detector(AnalogComparator::ON_FALLING_MODE, true);

// Ref. voltage: A0
// AnalogComparator detector(Board::A0, AnalogComparator::ON_FALLING_MODE);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalogComparator: started"));
  Watchdog::begin();
  AnalogPin::powerup();
  detector.enable();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  trace << event << endl;
}
