/**
 * @file CosaTermostat.ino
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
 * Demonstration of the Thermometer (DS18B20) periodic sampling
 * and Relay.
 *
 * @section Diagram
 *
 *  Thermometer              Relay
 *  +--------+            +---------+
 *  | sensor |            |  relay  |
 *  |        |---[temp]-->|  23.0 C |
 *  |        |      |     |  25.0 C |
 *  +--------+      |     +---------+
 *  [OWI/2048ms]    |
 *                  |        Probe
 *                  |     +---------+
 *                  |     |         |
 *                  +---->|         |
 *                        |         |
 *                        +---------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <UML.h>
#include <DS18B20.h>
#include <UML/Thermometer.h>

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

using namespace UML;

// Forward declaration of the connectors
extern Thermometer::Temperature temp;
extern Thermometer::Temperature min_temp;
extern Thermometer::Temperature max_temp;

// One-wire pin
OWI owi(Board::D4);

// The capsules with data dependencies (connectors)
Relay<Thermometer::Temperature> relay(temp, min_temp, max_temp, Board::LED);
Thermometer sensor(&owi, temp);

const char probe_name[] __PROGMEM = "temp";
Probe<Thermometer::Temperature> probe((str_P) probe_name, temp);

// The wiring; control dependencies (capsules)
Capsule* const temp_listeners[] __PROGMEM = { &relay, &probe, NULL };
Thermometer::Temperature temp(temp_listeners, 20.0);
Thermometer::Temperature min_temp(NULL, 23.0);
Thermometer::Temperature max_temp(NULL, 25.0);

void setup()
{
  // Start trace on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTermostat: started"));

  // Setup UML run-time
  UML::begin();

  // Connect and start the sensor
  sensor.connect(0);
  sensor.begin();
}

void loop()
{
  // Service Events and scheduled Capsules
  UML::service();
}
