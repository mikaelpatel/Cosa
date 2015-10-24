/**
 * @file CosaRS485slave.ino
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
 * RS485 slave example. Possible to run on Standard, Mighty and
 * Mega (UART). See also CosaRS485master.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <RS485.h>

#include "Cosa/IOBuffer.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

#if defined(BOARD_ATMEGA2560)
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"
static const uint8_t SLAVE = 0x01;
#define PORT 1
#else
static const uint8_t SLAVE = 0x02;
#define PORT 0
#endif

static IOBuffer<RS485::BUFFER_MAX> ibuf;
static IOBuffer<RS485::BUFFER_MAX> obuf;

RS485 rs485(PORT, &ibuf, &obuf, RS485::BUFFER_MAX, Board::LED, SLAVE);

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
  RTT::begin();

#if defined(TRACE)
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRS485slave: started"));
  TRACE(free_memory());
  TRACE(sizeof(RS485));
  ASSERT(rs485.begin(9600));
#else
  rs485.begin(9600);
#endif
}

void loop()
{
  static const uint32_t TIMEOUT = 1000L;
#if defined(TRACE)
  static uint16_t nr = 1;
#endif
  signal_t msg;
  int count;

  // Wait for a request signal
#if defined(TRACE)
  while (1) {
    count = rs485.recv(&msg, sizeof(msg), TIMEOUT);
    trace << nr << PSTR(":recv:count=") << count;
    if (count == sizeof(msg)) break;
    trace << endl;
  }
  trace << PSTR(",func=") << msg.func
	<< PSTR(",index=") << msg.param[0]
	<< PSTR(",nr=") << msg.param[1]
	<< endl;
#else
  while ((count = rs485.recv(&msg, sizeof(msg), TIMEOUT)) != sizeof(msg))
    ;
#endif

  // Decode function code and dispatch
  switch (msg.func) {
  case GET_MILLIS:
    msg.param[2] = RTT::millis();
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

#if defined(TRACE)
  trace << nr++ << PSTR(":send:count=") << count
	<< PSTR(",res=") << msg.param[2]
	<< endl;
#endif
}
