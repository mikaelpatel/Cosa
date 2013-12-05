/**
 * @file CosaISP.hh
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
 * Cosa AVR ISP (STK500 v1.x) serial programmer. Compile and upload to
 * an Arduino to turn it into an ISP. Connect device using the SPI
 * pins (MISO, MOSI, SCK, SS/RESET). In the Arduino IDL select
 * "Tools>Programmer>Arduino as ISP". Also select one of the Cosa 
 * device support cores (e.g. Cosa Tiny). 
 *
 * @section Trouble-shooting
 * If the Cosa Arduino ISP fails to synchronize please press RESET on
 * the board. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/AVR/Programmer.hh"
#include "Cosa/AVR/STK500.hh"

Programmer isp;
STK500 stk(&uart, &isp);

void setup()
{
  uart.begin(19200);
  Watchdog::begin();
}

void loop()
{
  stk.run();
}
