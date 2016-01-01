/**
 * @file CosaEEPROM.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Demo of the default EEPROM device driver and usage of the EEMEM
 * directive.
 *
 * @section Circuit
 * Uses the MCU internal EEPROM and Analog Pin A0 for samples.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/EEPROM.hh"
#include "Cosa/AnalogPin.hh"

// EEPROM access object
EEPROM eeprom;

// Simple configuration struct in EEPROM
static const int NAME_MAX = 16;
struct config_t {
  int mode;
  int speed;
  char name[NAME_MAX];
};
config_t config EEMEM = {
  17,
  9600,
  ".EEPROM"
};

// Sensor log in EEPROM
static const int DATA_MAX = 64;
uint16_t data[DATA_MAX] EEMEM;

// Analog input pin
AnalogPin sensor(Board::A0);

void setup()
{
  // Use serial as output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaEEPROM: started"));
  TRACE(free_memory());
  Watchdog::begin();

  // Initiate data vector with index
  for (uint8_t i = 0; i < DATA_MAX; i++)
    ASSERT(eeprom.write(&data[i], (uint16_t) 0xffff) == sizeof(uint16_t));

  // Read the configuration and print
  config_t init;
  ASSERT(eeprom.read(&init, &config, sizeof(init)) == sizeof(init));
  trace << PSTR("init(mode = ") << init.mode
	<< PSTR(", speed = ") << init.speed
	<< PSTR(", name = \"") << init.name
	<< PSTR("\")\n");

  AnalogPin::powerup();
}

void loop()
{
  static int i = 0;
  uint16_t x;

  // Print sensor samples
  if (i == 0) {
    for (i = 0; i < (int) membersof(data); i++) {
      ASSERT(eeprom.read(&x, &data[i]) == sizeof(x));
      trace << PSTR("data[") << i << PSTR("] = ") << x << endl;
    }
    i = 0;
    trace << endl;
  }

  // Update data element and write back
  x = sensor.sample();
  ASSERT(eeprom.write(&data[i], x) == sizeof(x));

  // Iterate to the next data element
  i += 1;
  if (i == membersof(data)) i = 0;

  // Take a nap
  sleep(2);
}
