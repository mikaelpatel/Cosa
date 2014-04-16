/**
 * @file CosaPCF8574.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Cosa PCF8574 Remote 8-bit I/O expander driver example.
 *
 * @section Circuit
 *                          PCF8574A
 *                       +------------+
 * (GND)---[ ]---------1-|A0       VCC|-16--------------(VCC)
 * (GND)---[ ]---------2-|A1       SDA|-15-----------(SDA/A4)
 * (GND)---[ ]---------3-|A2       SCL|-14-----------(SCL/A5)
 * (P0)----------------4-|P0       INT|-13
 * (P1)----------------5-|P1        P7|-12---------------(P7)
 * (P2)----------------6-|P2        P6|-11---------------(P6)
 * (P3)----------------7-|P3        P5|-10---------------(P5)
 * (GND)---------------8-|GND       P4|-9----------------(P4)
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/PCF8574.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// PCF8574 Remote 8-bit I/O expander with sub-address (0x27/0x4e)
PCF8574 port(0);

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPCF8574: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(TWI));
  TRACE(sizeof(port));

  // Start the watchdog ticks and RTC
  Watchdog::begin();
  RTC::begin();
  
  // Define an output pin for demo. All other are input default
  port.set_output_pin(0);
}

void loop()
{
  // Toggle led and output pin(0). Measure execution time in micro-seconds  
  static bool v = false;
  uint32_t start, stop;
  ledPin.toggle();

  start = RTC::micros(); 
  port.write((uint8_t) 0, v);
  stop = RTC::micros();
  trace << stop - start 
	<< PSTR(": write(0, ") << v << ')' 
	<< endl;
  v = !v;

  // Read input pins(1..7). Measure execution time in micro-seconds  
  for (uint8_t i = 1; i < 8; i++) {
    start = RTC::micros(); 
    bool u = port.read(i);
    stop = RTC::micros();
    trace << stop - start
	  << PSTR(": read(") << i << PSTR(") = ") << u
	  << endl;
  }

  ledPin.toggle();
  SLEEP(2);
}
