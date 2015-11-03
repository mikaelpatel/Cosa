/**
 * @file CosaDS18B20.ino
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
 * Cosa demonstration of the DS18B20 1-Wire device driver. Assumes
 * three thermometers are connected to 1-Wire bus on pin(D7). They may
 * be in parasite power mode.
 *
 * See LCD driver header files for circuit descriptions.
 *
 * @section Circuit
 * @code
 *                           DS18B20/3
 *                       +------------+++
 * (GND)---------------1-|GND         |||\
 * (D7)------+---------2-|DQ          ||| |
 *           |       +-3-|VDD         |||/
 *          4K7      |   +------------+++
 *           |       |
 * (VCC)-----+       +---(VCC/GND)
 *
 * @endcode

 * Connect Arduino to DS18B20 in D7 and GND. May use parasite
 * powering (connect DS18B20 VCC to GND) otherwise to VCC.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <OWI.h>
#include <DS18B20.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// One-wire pin and connected DS18B20 devices
#if defined(BOARD_ATTINY)
OWI owi(Board::D1);
#else
OWI owi(Board::D7);
#endif

// Support macro to create name strings in program memory
#define THERMOMETER(name)			\
const char name ## _PSTR[] __PROGMEM = #name;	\
DS18B20 name(&owi, name ## _PSTR)

// The devices connected to the one-wire bus
THERMOMETER(outdoors);
THERMOMETER(indoors);
THERMOMETER(basement);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS18B20: started"));

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(OWI));
  TRACE(sizeof(DS18B20));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // List connected devices
  trace << owi << endl;

  // Connect to the devices and print rom contents/trigger setting
  ledPin.toggle();
  DS18B20* temp[] = { &indoors, &outdoors, &basement };
  for (uint8_t i = 0; i < membersof(temp); i++) {
    DS18B20* t = temp[i];
    TRACE(t->connect(i));
    t->resolution(10);
    t->set_trigger(18, 22);
    t->write_scratchpad();
  }
  ledPin.toggle();
  for (uint8_t i = 0; i < membersof(temp); i++) {
    int8_t high, low;
    uint8_t resolution;
    DS18B20* t = temp[i];
    t->read_scratchpad();
    t->get_trigger(low, high);
    resolution = t->resolution();
    trace << (OWI::Driver&) *t << endl;
    trace << *t << endl;
    trace << PSTR("resolution = ") << resolution << endl;
    trace << PSTR("trigger = ") << low << PSTR("..") << high << endl;
    trace << endl;
  }
}

void loop()
{
  // Boardcast convert request to all devices
  ledPin.toggle();
  DS18B20::convert_request(&owi, 12, true);

  // Read back results; first will wait for the conversion to complete
  indoors.read_scratchpad();
  outdoors.read_scratchpad();
  basement.read_scratchpad();

  // Print measurement with device name
  trace << indoors << PSTR(", ") << outdoors << PSTR(", ") << basement << endl;

  // Do an alarm search and print alarmed devices
  DS18B20::convert_request(&owi, 12, true);
  DS18B20::Search iter(&owi);
  DS18B20* temp;
  while ((temp = iter.next()) != 0) {
    trace << PSTR("ALARM:") << *temp << endl;
  }

  // Sleep before requesting a new sample
  sleep(2);
}
