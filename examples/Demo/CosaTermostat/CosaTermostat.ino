/**
 * @file CosaTermostat.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Cosa demonstration of the DHT11 device driver and simple relay
 * control.
 *
 * @section Circuit
 * 1. Connect Arduino/ATtiny to DHT11 module, D7/D1 => DHT data pin 
 * with required pullup resistor. Connect power (VCC) and ground (GND).   
 * 2. Connect Arduino/ATtiny to Relay module, D6/D2 => relay#1, 
 * D5/D3 => relay#2.
 * 3. Soft UART ATtiny D0 => UART RX.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Driver/DHT11.hh"

// Sensor and relay control pins
#if defined(__ARDUINO_TINYX5__)
DHT11 sensor(Board::D1);
OutputPin heater(Board::D2, 1);
OutputPin fan(Board::D3, 1);
#else
DHT11 sensor(Board::D7);
OutputPin heater(Board::D6, 1);
OutputPin fan(Board::D5, 1);
#endif

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
}

void loop()
{
  // Read error count; Turn off fan and heater if max error (60 sec).
  static uint8_t err = 0;
  static const uint8_t ERR_MAX = 30;

  // Read temperature and humidity. Handle read errors
  int16_t temperature, humidity;
  if (!sensor.read(humidity, temperature)) {
    trace.print_P(PSTR("sensor: FAILED\n"));
    err += 1;
    if (err > ERR_MAX) {
      heater.set();
      fan.set();
      err = 0;
    }
    return;
  }
  trace.print_P(PSTR("sensor:  "));
  trace.printf_P(PSTR("RH = %d%%, T = %d C\n"), humidity, temperature);

  // Check if heater should be turned on @ 22 C and off @ 26 C
  static bool heating = false;
  static uint32_t hours = 0;
  static uint8_t minutes = 0;
  static uint8_t seconds = 0;
  static const uint8_t TEMP_MIN = 22;
  static const uint8_t TEMP_MAX = 26;
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
      trace.print_P(PSTR("Heater OFF\n"));
      trace.printf_P(PSTR("Runtime: %l:%d:%d\n"), hours, minutes, seconds);
      heater.set();
      heating = 0;
    }
  }
  else if (temperature < TEMP_MIN) {
    trace.print_P(PSTR("Heater ON\n"));
    heater.clear();
    heating = 1;
  }

  // Check if fan should be turned on @ 70 RH and off @ 50 RH
  static bool venting = false;
  static const uint8_t RH_MIN = 50;
  static const uint8_t RH_MAX = 70;
  if (venting) {
    if (humidity < RH_MIN) {
      trace.print_P(PSTR("Fan OFF\n"));
      fan.set();
      venting = false;
    }
  }
  else if (humidity > RH_MAX) {
    trace.print_P(PSTR("Fan ON\n"));
    fan.clear();
    venting = true;
  }

  // Sample every 2 seconds
  SLEEP(2);
}
