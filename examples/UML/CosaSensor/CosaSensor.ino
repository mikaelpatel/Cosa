/**
 * @file CosaSensor.ino
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
 * Demonstration of the Sensor sampling and Probe.
 *
 * @section Diagram
 *
 *    Sensor              Probe
 *  +--------+          +--------+
 *  | sensor |          | probe  |
 *  |        |---[s1]-->|        |
 *  |        |          |        |
 *  +--------+          +--------+
 *  [A0/1024ms]
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

#include "Cosa/UML/Sensor.hh"
#include "Cosa/UML/Probe.hh"
#include "Cosa/UML/Controller.hh"

using namespace UML;

// Forward declaration of the connectors
extern Sensor::Sample s1;

// The capsules with data dependencies (connectors)
Sensor sensor(Board::A0, s1, 1024);

const char probe_name[] __PROGMEM = "probe";
Probe<Sensor::Sample> probe((str_P) probe_name, s1);

// The wiring; control dependencies (capsules)
Capsule* const s1_listeners[] __PROGMEM = { &probe, NULL };
Sensor::Sample s1(s1_listeners, 0);

void setup()
{
 // Start trace on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSensor: started"));

  // Use the watchdog for timeout events
  Watchdog::begin(16, Watchdog::push_timeout_events);

  // Start the timed probe
  sensor.begin();
}

void loop()
{
  Event::service();
  controller.run();
}
