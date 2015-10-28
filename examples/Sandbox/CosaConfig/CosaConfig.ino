/**
 * @file CosaConfig.ino
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
 * Cosa local configuration demonstration and tests.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa.h"
#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaConfig: started"));

  sleep(1);
  int available = uart.available();
  int room = uart.room();
  TRACE(available);
  TRACE(room);

#ifdef COSA_BOD_DISABLE
  trace << PSTR("COSA_BOD_DISABLE defined") << endl;
#endif
  TRACE(COSA_EVENT_QUEUE_MAX);
  TRACE(COSA_UART_BUFFER_MAX);
  TRACE(COSA_SOFT_UART_BUFFER_MAX);
}

void loop()
{
  ASSERT(true == false);
}

