/**
 * @file CosaSi7021.ino
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
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Si70XX.h>
#include "Cosa/RTT.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

Si70XX sensor;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSi70XX: started"));
  RTT::begin();

  uint8_t reg;
  ASSERT(sensor.read_user_register(reg));
  trace << PSTR("user: ");
  trace.print((uint16_t) reg, 8, IOStream::bin);
  trace.println();

  uint8_t snr[8];
  ASSERT(sensor.read_electronic_serial_number(snr));
  trace << PSTR("snr: ");
  trace.print(snr, sizeof(snr), IOStream::hex);
  trace << PSTR("device: ") << hex << snr[4] << endl;

  uint8_t rev;
  ASSERT(sensor.read_firmware_revision(rev));
  trace << PSTR("rev: ") << hex << rev << endl;
}

void loop()
{
  periodic(timer, 2000L) {
    float humidity, humidity_temperature, temperature;

    MEASURE("measure-read humidity: ", 1) {
      sensor.measure_humidity();
      humidity = sensor.read_humidity();
    }

    MEASURE("measure humidity: ", 1) {
      sensor.measure_humidity();
    }
    delay(20);
    MEASURE("read humidity: ", 1) {
      humidity = sensor.read_humidity();
    }

    MEASURE("read humidity temperature: ", 1)
      humidity_temperature = sensor.read_humidity_temperature();

    MEASURE("measure-read temperature: ", 1) {
      sensor.measure_temperature();
      temperature = sensor.read_temperature();
    }

    MEASURE("measure temperature: ", 1) {
      sensor.measure_temperature();
    }
    delay(10);
    MEASURE("read temperature: ", 1) {
      temperature = sensor.read_temperature();
    }

    trace << humidity << PSTR("% RH, ")
	  << humidity_temperature << PSTR("° C, ")
	  << temperature << PSTR("° C")
	  << endl;
  }
  yield();
}
