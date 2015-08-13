/**
 * @file CosaMCP23008.ino
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
 * Cosa MCP23008 Remote 8-bit I/O expander driver example. Includes
 * benchmarking of read/write of port and pins (100/444 KHz);
 *  1. read pin, 472(168) us
 *  2. read pin (cached address), 236(88) us
 *  3. read pin (read sequence 16), 108(33) us
 *  4. write pin, 340(120) us
 *  5. pulse pin (write 2*8), 676(234) us
 *  6. pulse pin (write sequence 2*8), 226(66) us
 *
 * @section Circuit
 * @code
 *                          MCP23008
 *                       +------------+
 * (SCL)---------------1-|SCL      VCC|-18--------------(VCC)
 * (SDA)---------------2-|SDA      GP7|-17---------------(P7)
 * (GND)---------------3-|A2       GP6|-16---------------(P6)
 * (GND)---------------4-|A1       GP5|-15---------------(P5)
 * (GND)---------------5-|A0       GP4|-14---------------(P4)
 * (RST)---------------6-|RESET/   GP3|-13---------------(P3)
 *                     7-|NC       GP2|-12---------------(P2)
 * (EXT)---------------8-|INT      GP1|-11---------------(P1)
 * (GND)---------------9-|GND      GP0|-10---------------(P0)
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <MCP23008.h>

#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Use TWI at max frequency (444 KHz @ 16 MHz)
#define USE_MAX_FREQ

// MCP23008 Remote 8-bit I/O expander with sub-address(0x0)
MCP23008 port(0);

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(250000);
  trace.begin(&uart, PSTR("CosaMCP23008: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(TWI));
  TRACE(sizeof(port));

  // Start the watchdog ticks and RTC
  Watchdog::begin();
  RTC::begin();

#if defined(USE_MAX_FREQ)
  // Set max frequency
  twi.set_freq(TWI::MAX_FREQ);
#endif

  // Start the MCP23008 device driver
  port.begin();

  // Define an output pin(0) for demo. All other are input default
  port.set_output_pin(0);

  // Use pullup resistor on pin(1..7)
  port.set_pullup(0xfe);
}

void loop()
{
  // Toggle led and output pin(0). Measure execution time in micro-seconds
  static bool v = false;
  static const uint8_t p = 0;
  uint32_t start, stop;
  ledPin.toggle();

  // Fast toggle pin(0)
  uint8_t buf[16];
  for (uint8_t i = 0; i < sizeof(buf); i++) buf[i] = i & 1;
  start = RTC::micros();
  port.write(buf, sizeof(buf));
  stop = RTC::micros();
  trace << (stop - start) / sizeof(buf)
	<< PSTR(": write(buf[") << sizeof(buf) << PSTR("])")
	<< endl;
  trace.flush();

  // Toggle pin(0)
  start = RTC::micros();
  for (uint8_t i = 0; i < sizeof(buf); i++) {
    port.write(p, i & 1);
  }
  stop = RTC::micros();
  trace << (stop - start) / sizeof(buf)
	<< PSTR(": write():") << sizeof(buf) << PSTR("X")
	<< endl;
  trace.flush();

  // Read input pins
  start = RTC::micros();
  uint8_t u = port.read();
  stop = RTC::micros();
  trace << stop - start << PSTR(": read() = ") << bin << u << endl;
  trace.flush();

  // Fast read pins
  start = RTC::micros();
  port.read(buf, sizeof(buf));
  stop = RTC::micros();
  trace << (stop - start) / sizeof(buf)
	<< PSTR(": read(buf[") << sizeof(buf) << PSTR("])")
	<< endl;
  trace.flush();

  // Toggle pin(0)
  start = RTC::micros();
  port.write((uint8_t) p, v);
  stop = RTC::micros();
  v = !v;
  trace << stop - start	<< PSTR(": write(") << p << ',' << v << ')' << endl;
  trace.flush();

  // Read each pin(0..7)
  for (uint8_t pin = 0; pin < 8; pin++) {
    start = RTC::micros();
    u = port.read(pin);
    stop = RTC::micros();
    trace << stop - start << PSTR(": read(") << pin << PSTR(") = ") << u << endl;
    trace.flush();
  }
  ledPin.toggle();
  sleep(1);
}
