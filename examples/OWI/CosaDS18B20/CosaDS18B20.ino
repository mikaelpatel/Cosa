/**
 * @file CosaDS18B20.ino
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa demonstration of the DS18B20 1-Wire device driver.
 * Assumes three thermometers are connected to 1-Wire bus on
 * pin(7) in search order. Will not work in parasite power mode.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// One-wire pin and connected DS18B20 devices
#if defined(__ARDUINO_TINY__)
OWI owi(Board::D1);
#else
OWI owi(Board::D7);
#endif

// Support macro to create name strings in program memory
#define THERMOMETER(name)			\
  const char name ## _PSTR[] PROGMEM = #name;	\
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
    int8_t high, low;
    uint8_t resolution;
    DS18B20* t = temp[i];
    t->connect(i);
    t->get_trigger(high, low);
    resolution = t->get_resolution();
    trace << (OWI::Driver&) *t << endl;
    trace << PSTR("resolution = ") << resolution << endl;
    trace << PSTR("trigger = ") << high << ':' << low << endl;
    t->set_trigger(30, 20);
    t->write_scratchpad();
    trace << endl;
  }
  ledPin.toggle();
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
  ledPin.toggle();

  // Do an alarm search
  DS18B20::Search iter(&owi);
  DS18B20* temp;
  while ((temp = iter.next()) != 0) {
    trace << PSTR("ALARM:") << *temp << endl;
  }

  // Do alarm dispatch; Calls on_alarm() for each device with alarm set
  owi.alarm_dispatch();

  // Sleep before requesting a new sample
  SLEEP(1);
}
