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
#include "Cosa/FixedPoint.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// One-wire pin and connected DS18B20 devices
#if defined(__ARDUINO_TINY__)
OWI owi(Board::D1);
#else
OWI owi(Board::D7);
#endif
DS18B20 outdoors(&owi);
DS18B20 indoors(&owi);
DS18B20 basement(&owi);

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// Print the current temperature reading
void print_temperature_P(const char* prefix, DS18B20& thermometer)
{
  FixedPoint temp(thermometer.get_temperature(), 4);
  int16_t integer = temp.get_integer();
  uint16_t fraction = temp.get_fraction(4);
  trace << prefix << integer << '.';
  if ((fraction != 0) && (fraction < 1000)) trace << '0';
  trace << fraction;
}

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS18B20: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // List connected devices
  owi.print_devices();

  // Connect to the devices and print rom contents
  ledPin.toggle();
  TRACE(indoors.connect(0));
  indoors.print_rom();
  TRACE(outdoors.connect(1));
  outdoors.print_rom();
  TRACE(basement.connect(2));
  basement.print_rom();
  ledPin.toggle();

  // Start the watchdog ticks counter with 16 ms period
  Watchdog::begin(16);

  // Start the conversion pipeline; indoors->outdoors->basement sampling
  indoors.convert_request();
}

void loop()
{
  // Start outdoors temperature conversion and read the indoors temperature
  ledPin.toggle();
  outdoors.convert_request();
  indoors.read_scratchpad();
  print_temperature_P(PSTR("indoors = "), indoors);
  ledPin.toggle();
  SLEEP(1);

  // Start basement temperature conversion and read the outdoors temperature
  ledPin.toggle();
  basement.convert_request();
  outdoors.read_scratchpad();
  print_temperature_P(PSTR(", outdoors = "), outdoors);
  ledPin.toggle();
  SLEEP(1);

  // Start indoors temperature conversion and read the basement temperature
  ledPin.toggle();
  indoors.convert_request();
  basement.read_scratchpad();
  print_temperature_P(PSTR(", basement = "), basement);
  trace.println();
  ledPin.toggle();
  SLEEP(1);
}
