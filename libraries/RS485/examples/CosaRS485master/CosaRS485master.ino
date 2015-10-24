/**
 * @file CosaRS485master.ino
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
 * RS485 master example for Mega/Mighty. See also CosaRS485slave.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <RS485.h>

#include "Cosa/IOBuffer.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"

#if !defined(USART1_UDRE_vect)
#error CosaRS485master: board not supported.
#endif

static const uint8_t PORT = 1;
static const uint8_t SLAVE = 0x02;

static IOBuffer<RS485::BUFFER_MAX> ibuf;
static IOBuffer<RS485::BUFFER_MAX> obuf;

RS485 rs485(PORT, &ibuf, &obuf, RS485::BUFFER_MAX, Board::LED);

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
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRS485master: started"));
  TRACE(free_memory());
  TRACE(sizeof(RS485));
  ASSERT(rs485.begin(9600));
}

void loop()
{
  static const uint32_t TIMEOUT = 500L;
  static uint16_t errors = 0;
  static uint8_t nr = 0;
  static uint8_t func = GET_MILLIS;
  static uint8_t dest = 1;
  signal_t msg;
  int count;

  // Send signal and receive response
  msg.func = func;
  msg.param[0] = 0;
  msg.param[1] = nr;
  count = rs485.send(&msg, sizeof(msg), dest);
  trace << nr << PSTR(":send:")
	<< PSTR("dest=") << dest
	<< PSTR(",count=") << count
	<< PSTR(",func=") << func
	<< PSTR(",index=") << msg.param[0]
	<< PSTR(",nr=") << msg.param[1]
	<< endl;
  count = rs485.recv(&msg, sizeof(msg), TIMEOUT);
  trace << nr++ << PSTR(":recv:")
	<< PSTR("count=") << count
	<< PSTR(",func=") << msg.func
	<< PSTR(",index=") << msg.param[0]
	<< PSTR(",nr=") << msg.param[1];

  // Check for errors
  if (count <= 0) {
    trace << PSTR(",errno=") << count
	  << PSTR(",errors=") << ++errors;
  }

  // Print signal response
  else {
    if (msg.func == GET_MILLIS) {
      trace << PSTR(",millis=");
    }
    else if (msg.func == GET_HUMIDITY) {
      trace << PSTR(",humidity=");
    }
    else if (msg.func == GET_TEMPERATURE) {
      trace << PSTR(",temperature=");
    }
    trace << msg.param[2];
  }
  trace << endl;

  // Step to next function code and on full cycle step to next destination
  func += 1;
  if (func > GET_TEMPERATURE) {
    func = GET_MILLIS;
    dest += 1;
    if (dest > 2) dest = 1;
  }
  Watchdog::delay(500);
}
