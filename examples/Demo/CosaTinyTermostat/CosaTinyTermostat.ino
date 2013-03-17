/**
 * @file CosaTinyTermostat.ino
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
 * Demonstrate soft uart for the ATtiny. Trace output from
 * ATtiny. This is an ATtiny version of the termostat demo.
 *
 * @section Circuit
 * Connect DHT11 sensor to ATtiny D1, LED or relay to D2, and D3.
 * ATtiny D0 is trace output (soft uart) and should be connected
 * to an Arduino running the CosaTinyMonitor.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Board.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
#include "Cosa/Driver/DHT11.hh"

DHT11 sensor(Board::D1);
OutputPin heater(Board::D2);
OutputPin fan(Board::D3);

void setup()
{
  // Start real-time clock and watchdog. 
  RTC::begin();
  Watchdog::begin();

  // Wait for internal clock to stabilize
  SLEEP(1);

  // Start soft uart and bind to trace stream
  uart.begin();
  trace.begin(&uart, PSTR("CosaTinyUART: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check that real-time clock and delay is working
  uint32_t start = RTC::micros();
  DELAY(100);
  uint32_t stop = RTC::micros();
  uint16_t delay = stop - start;
  trace.printf_P(PSTR("delay = %ud\n"), delay);
}

void loop()
{
  uint8_t temperature, humidity;
  if (!sensor.read(temperature, humidity)) return;
  trace.printf_P(PSTR("RH = %d%%, T = %d C\n"), humidity, temperature);

  // Check if heater should be turned on @ 22 C and off @ 26 C
  static uint8_t heating = 0;
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
      heater.off();
      heating = 0;
    }
  }
  else if (temperature < TEMP_MIN) {
    trace.print_P(PSTR("Heater ON\n"));
    heater.on();
    heating = 1;
  }

  // Check if fan should be turned on @ 70 RH and off @ 50 RH
  static uint8_t venting = 0;
  static const uint8_t RH_MIN = 50;
  static const uint8_t RH_MAX = 70;
  if (venting) {
    if (humidity < RH_MIN) {
      trace.print_P(PSTR("Fan OFF\n"));
      fan.off();
      venting = 0;
    }
  }
  else if (humidity > RH_MAX) {
    trace.print_P(PSTR("Fan ON\n"));
    fan.on();
    venting = 1;
  }
  SLEEP(2);
}
