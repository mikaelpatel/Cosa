/**
 * @file CosaRS485slave.ino
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
 * @section Description
 * RS485 slave example for Mega/Mighty. See also CosaRS485master.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream/Driver/RS485.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Memory.h"

// Communication channel on UART(1)
static const uint8_t SLAVE = 0x02;
RS485 rs485(1, Board::LED, SLAVE);

// Simulated sensors; analog pins (0 and 1)
AnalogPin humidity(Board::A0);
AnalogPin temperature(Board::A1);

// Function codes
enum {
  GET_MILLIS,
  GET_HUMIDITY,
  GET_TEMPERATURE
};

struct signal_t {
  static const uint8_t PARAM_MAX = 3; // Max number of parameters/results
  uint8_t func;			      // Function code
  uint16_t param[PARAM_MAX];	      // Parameter/result block
				      // 0: index
				      // 1: sequence number
				      // 2: value
};

void setup()
{
  Watchdog::begin();
  RTC::begin();
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRS485slave: started"));
  TRACE(free_memory());
  TRACE(sizeof(RS485));
  ASSERT(rs485.begin(9600));
}

void loop()
{
  static const uint32_t TIMEOUT = 1000L;
  static uint16_t nr = 1;
  signal_t msg;
  int count;

  // Wait for a request signal
  while (1) {
    count = rs485.recv(&msg, sizeof(msg), TIMEOUT);
    trace << nr << PSTR(":recv:count=") << count;
    if (count == sizeof(msg)) break;
    trace << endl;
  }
  
  // Decode function code and dispatch
  trace << PSTR(",func=") << msg.func 
	<< PSTR(",index=") << msg.param[0] 
	<< PSTR(",nr=") << msg.param[1] 
	<< endl;
  switch (msg.func) {
  case GET_MILLIS:
    msg.param[2] = RTC::millis();
    break;
  case GET_HUMIDITY:
    msg.param[2] = humidity.sample(); 
    break;
  case GET_TEMPERATURE:
    msg.param[2] = temperature.sample();
    break;
  default:
    return;
  }
  count = rs485.send(&msg, sizeof(msg));
  trace << nr++ << PSTR(":send:count=") << count 
	<< PSTR(",res=") << msg.param[2]  
	<< endl;
}
