/**
 * @file CosaMegaUARTs.ino
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
 * Cosa demonstration of how to use that additional UARTs on the
 * Arduino Mega. The sketch allocates buffers and initiates usage of
 * UART1. The IOStream::Device is connected to an IOStream, cout.
 * The sketch will print a period on cout and when characters are
 * available, transfer them to the trace iostream.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#if !defined(USART1_UDRE_vect)
#error CosaMegaUARTs: board not supported.
#endif
#include "Cosa/Trace.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"

// Create buffer for UART1
static IOBuffer<UART::RX_BUFFER_MAX> ibuf;
static IOBuffer<UART::TX_BUFFER_MAX> obuf;

// Create UART1 and bind to the cout IOStream
UART uart1(1, &ibuf, &obuf);
IOStream cout(&uart1);

void setup()
{
  // Start the normal trace output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMegaUARTs: started"));

  // Start the additional UART
  uart1.begin(9600);
  cout << PSTR("Running....");

  // Watchdog for sleeping
  Watchdog::begin();
}

void loop()
{
  // Transfer character from UART1 to trace
  while (uart1.available()) trace << (char) uart1.getchar();
}

