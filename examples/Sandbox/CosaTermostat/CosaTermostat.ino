/**
 * @file CosaTermostat.ino
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
 * Cosa demonstration of the DHT11 device driver and simple relay
 * control.
 *
 * @section Circuit
 * 1. Connect Arduino/ATtiny to DHT11 module, EXT0 => DHT data pin
 * with required pullup resistor. Connect power (VCC) and ground (GND).
 * 2. Connect Arduino/ATtiny to Relay module, D6/D1 => relay#1,
 * D5/D3 => relay#2.
 * 3. Soft UART ATtiny D0 => UART RX.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

#include <DHT.h>

// Sensor and relay control pins
OutputPin led(Board::LED);
DHT11 sensor(Board::EXT0);
#if defined(BOARD_ATTINY)
OutputPin heater(Board::D1, 1);
OutputPin fan(Board::D3, 1);
#else
OutputPin heater(Board::D6, 1);
OutputPin fan(Board::D5, 1);
#endif

str_P
PSTATE(uint8_t state)
{
  return (state ? PSTR(" on") : PSTR(" off"));
}

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTermostat: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(DHT11));

  // Start the watchdog ticks and sensor monitoring
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  // Read error count; Turn off fan and heater if max error (60 sec).
  static uint8_t err = 0;
  static const uint8_t ERR_MAX = 30;

  // Read temperature and humidity. Handle read errors
  int16_t temperature, humidity;
  if (!sensor.sample(humidity, temperature)) {
    trace << PSTR("sensor: FAILED") << endl;
    err += 1;
    if (err > ERR_MAX) {
      heater.set();
      fan.set();
      err = 0;
    }
    sleep(2);
    return;
  }

  // Check if heater should be turned on @ T 22 C and off @ T 26 C
  static bool heating = false;
  static uint32_t hours = 0;
  static uint8_t minutes = 0;
  static uint8_t seconds = 0;
  static const int16_t TEMP_MIN = 220;
  static const int16_t TEMP_MAX = 260;
  if (heating) {
    seconds += 2;
    if (seconds == 60) {
      seconds = 0;
      minutes += 1;
      if (minutes == 60) {
	minutes = 0;
	hours += 1;
      }
    }
    if (temperature > TEMP_MAX) {
      trace << PSTR("Heater OFF") << endl;
      trace.printf(PSTR("Runtime: %l:%d:%d\n"), hours, minutes, seconds);
      heater.set();
      heating = 0;
    }
  }
  else if (temperature < TEMP_MIN) {
    trace << PSTR("Heater ON") << endl;
    heater.clear();
    heating = 1;
  }

  // Check if fan should be turned on @ RH 70% and off @ RH 50%
  static bool venting = false;
  static const int16_t RH_MIN = 500;
  static const int16_t RH_MAX = 700;
  if (venting) {
    if (humidity < RH_MIN) {
      trace << PSTR("Fan OFF") << endl;
      fan.set();
      venting = false;
    }
  }
  else if (humidity > RH_MAX) {
    trace << PSTR("Fan ON") << endl;
    fan.clear();
    venting = true;
  }

  trace << sensor
	<< PSTR(", Heater = ") << heating
	<< PSTR(", Fan = ") << venting
	<< endl;

  // Sample every 2 seconds
  sleep(2);
}
