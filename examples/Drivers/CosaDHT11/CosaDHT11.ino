/**
 * @file CosaDHT11.ino
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
 * Cosa demonstration of the DHT11/DHT22 device driver.
 *
 * @section Circuit
 * Connect Arduino to DHT11 (indoors), D6 => DHT data pin, 
 * DHT22 (outdoors), D7 => DHT data pin. Connect power (VCC) 
 * and ground (GND).   
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Driver/DHT11.hh"
#include "Cosa/Driver/DHT22.hh"

// Test with two DHT11 instead of DHT11 and DHT22
#define DHT11_ONLY

OutputPin ledPin(Board::LED);
#if defined(__ARDUINO_TINY__)
DHT11 indoors(Board::D1);
DHT22 outdoors(Board::D2);
#else
DHT11 indoors(Board::D6);
DHT22 outdoors(Board::D7);
#endif

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHT11: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(DHT11));
  TRACE(sizeof(DHT22));

  // Start the watchdog for low power sleep
  Watchdog::begin();

  // Adjust the indoors device with -1 humidity and +1 temperature
  indoors.calibrate(-1,1);
}

void loop()
{
  // Sample every 2 seconds
  SLEEP(2);

  // Read in- and outdoors temperature and humidity
  ledPin.toggle();
  int16_t humidity;
  int16_t temperature;
  if (indoors.read(humidity, temperature)) {
    trace.print_P(PSTR("indoors:  "));
    trace.printf_P(PSTR("RH = %d%%, T = %d C\n"), humidity, temperature);
  }
  if (outdoors.read(humidity, temperature)) {
#if defined(DHT11_ONLY)
    humidity = 10 * swap(humidity);
    temperature = 10 * swap(temperature);
#endif
    trace.print_P(PSTR("outdoors: "));
    trace.printf_P(PSTR("RH = %d%%, T = %d C\n"), humidity / 10, temperature / 10);
  }

  // Blink built-in led during active period
  ledPin.toggle();
}
