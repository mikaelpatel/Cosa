/**
 * @file CosaTrigger.ino
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
 * Demonstration sketch of Trigger, Counter and TimedProbe; the
 * Trigger will update a connector from an ISR and schedule a Counter.
 * The number of signals from the Trigger is probed using the
 * TimedProbe so that the value is printed every other second.
 *
 * @section Diagram
 *
 *    Trigger              Counter           TimedProbe
 *  +---------+          +---------+         +---------+
 *  | trigger |          | counter |         |  probe  |
 *  |         |---[s1]-->|         |--[c1]-->|         |
 *  |         |          |         |         |         |
 *  +---------+          +---------+         +---------+
 *       [EXT0]
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <UML.h>

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

using namespace UML;

// Define Counter type
typedef Counter<Signal> Triggers;

// Forward declaration of the connectors
extern Signal s1;
extern Triggers::Count c1;

// The capsules with data dependencies (connectors)
Trigger trigger(Board::EXT0, s1);
Triggers counter(s1, c1);

const char probe_name[] __PROGMEM = "probe";
TimedProbe<Triggers::Count> probe((str_P) probe_name, c1);

// The wiring; control dependencies
Capsule* const s1_listeners[] __PROGMEM = { &counter, NULL };
Signal s1(s1_listeners, 0);

Capsule* const c1_listeners[] __PROGMEM = { NULL };
Triggers::Count c1(c1_listeners, 0L);

void setup()
{
 // Start trace on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTrigger: started"));

  // Start UML run-time
  UML::begin();

  // Start the Timed Probe
  probe.begin();

  // Enable the Trigger Capsule
  trigger.enable();
}

void loop()
{
  // Service Events and scheduled Capsules
  UML::service();
}
