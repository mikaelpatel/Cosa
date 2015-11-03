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
 * Cosa MCP23008 Remote 8-bit I/O expander driver benchmark.
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
 *                    -6-|RESET/   GP3|-13---------------(P3)
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
#include "Cosa/RTT.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// Use TWI at max frequency (800 KHz @ 16 MHz)
#define USE_MAX_FREQ

// MCP23008 Remote 8-bit I/O expander with sub-address(0x0)
MCP23008 port;

// Use the built-in led
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMCP23008: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(TWI));
  TRACE(sizeof(port));

  // Start the watchdog ticks and RTT
  Watchdog::begin();
  RTT::begin();

#if defined(USE_MAX_FREQ)
  // Set max frequency
  twi.set_freq(TWI::MAX_FREQ);
#endif

  // Start the MCP23008 device driver
  ASSERT(port.begin());

  // Define an output pin(0) for demo. All other are input default
  ASSERT(port.output_pin(0));

  // Use pullup resistor on pin(1..7)
  ASSERT(port.pullup(0xfe));
}

void loop()
{
  static bool v = false;
  static const uint8_t p = 0;
  static const size_t BUF_MAX = 10;
  uint8_t buf[BUF_MAX];
  ledPin.toggle();

  MEASURE("1:Read input pins:", 1)
    v = port.read();

  MEASURE("2:Read input pins (cached address):", 1)
    v = port.read();

  MEASURE("3:Read pins (10X):", 1)
    for (uint8_t i = 0; i < BUF_MAX; i++)
      v = port.read();

  MEASURE("4:Read pins (buf[10]):", 1)
    port.read(buf, BUF_MAX);

  MEASURE("4:Write pins:", 1)
    port.write(0);

  MEASURE("5:Write pins (10X):", 1) {
    for (uint8_t i = 0; i < BUF_MAX; i++)
      port.write(i & 1);
  }

  for (uint8_t i = 0; i < BUF_MAX; i++) buf[i] = i & 1;
  MEASURE("6:Write pins (buf[10]:", 1)
    port.write(buf, sizeof(buf));

  MEASURE("7:Read pin(p):", 1)
    v = port.read_pin(p);
  v = !v;

  MEASURE("8:Write pin(p,v):", 1)
    port.write_pin(p, v);

  ledPin.toggle();
  trace << endl;
  sleep(1);
}

/**
 * @section Measurements (100 KHz)
 *
98:void loop():measure:1:Read input pins:472 us
101:void loop():measure:2:Read input pins (cached address):236 us
104:void loop():measure:3:Read pins (10X):2360 us
108:void loop():measure:4:Read pins (buf[10]):1136 us
111:void loop():measure:4:Write pins:340 us
114:void loop():measure:5:Write pins (10X):3388 us
120:void loop():measure:6:Write pins (buf[10]:1220 us
123:void loop():measure:7:Read pin(p):472 us
127:void loop():measure:8:Write pin(p,v):340 us
 *
 * @section Measurements (800 KHz)
 *
98:void loop():measure:1:Read input pins:128 us
101:void loop():measure:2:Read input pins (cached address):64 us
104:void loop():measure:3:Read pins (10X):644 us
108:void loop():measure:4:Read pins (buf[10]):248 us
111:void loop():measure:4:Write pins:88 us
114:void loop():measure:5:Write pins (10X):884 us
120:void loop():measure:6:Write pins (buf[10]:260 us
123:void loop():measure:7:Read pin(p):124 us
127:void loop():measure:8:Write pin(p,v):88 us
 */
