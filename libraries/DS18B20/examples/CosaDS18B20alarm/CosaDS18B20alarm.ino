/**
 * @file CosaDS18B20alarm.ino
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
 * Cosa demonstration of the DS18B20 1-Wire device driver alarm
 * callback. Assumes three thermometers are connected to 1-Wire bus on
 * pin(D7). They may be in parasite power mode.
 *
 * Note that temperature is only read on an alarm. A conversion
 * request is issued every 5 seconds. The request is boardcasted to all
 * connected devices (i.e. a single command is sent without rom
 * address).
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
 *                       TinyRTC(DS1307)
 *                       +------------+
 *                     1-|SQ          |
 *                     2-|DS        DS|-1
 * (A5/SCL)------------3-|SCL      SCL|-2
 * (A4/SDA)------------4-|SDA      SDA|-3
 * (VCC)---------------5-|VCC      VCC|-4
 * (GND)---------------6-|GND      GND|-5
 *                     7-|BAT         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <OWI.h>
#include <DS18B20.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// One-wire pin and connected DS18B20 devices
#if defined(BOARD_ATTINY)
OWI owi(Board::D1);
#else
OWI owi(Board::D7);
#define USE_RTC
#endif

#ifdef USE_RTC
#include <DS1307.h>
DS1307 rtc;
time_t now;
#endif

// Simple alarm callback handler
class Thermometer : public DS18B20 {
public:
  Thermometer(OWI* owi, const char* name) : DS18B20(owi, name) {}
  virtual void on_alarm();
};

void
Thermometer::on_alarm()
{
#ifndef USE_RTC
  uint32_t now = Watchdog::millis();
#endif

  // Read and print temperature. Do not need reset and presence pulse
  read_scratchpad(false);
  trace << now << PSTR(" alarm ") << *this << PSTR(" C") << endl;
}

// Support macro to create name strings in program memory for devices
#define THERMOMETER(var)					\
  const char var ## _name[] __PROGMEM = #var;			\
  Thermometer var(&owi, var ## _name)

// The devices connected to the one-wire bus
THERMOMETER(outdoors);
THERMOMETER(indoors);
THERMOMETER(basement);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS18B20alarm: started"));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Connect to the devices and set trigger thresholds
  indoors.connect(0);
  indoors.set_trigger(18, 20);
  indoors.write_scratchpad();
  trace << (OWI::Driver&) indoors << endl;

  outdoors.connect(1);
  outdoors.set_trigger(-10, 30);
  outdoors.write_scratchpad();
  trace << (OWI::Driver&) outdoors << endl;

  basement.connect(2);
  basement.set_trigger(4, 20);
  basement.write_scratchpad();
  trace << (OWI::Driver&) basement << endl;
}

void loop()
{
  // Turn the builtin led on while working
  ledPin.toggle();

#ifdef USE_RTC
  // Get the current time for logging
  rtc.get_time(now);
  now.to_binary();
#endif

  // Issue a convert request and check for alarms
  DS18B20::convert_request(&owi, 12, true);
  owi.alarm_dispatch();

  // Turn the builtin led off and sleep
  ledPin.toggle();
  sleep(5);
}
