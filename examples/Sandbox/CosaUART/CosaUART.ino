/**
 * @file CosaUART.ino
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
 * Cosa IOStream::Device UART line scan example.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  // Start the UART and Watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaUART: started"));
  Watchdog::begin();
}

void loop()
{
  // Count the number of wake-ups
  static uint32_t n = 0;
  static bool is_scan_mode = false;
  
  // Sleep and wait for something to happen
  Power::sleep(SLEEP_MODE_IDLE);
  n += 1;
  
  // Check if a complete line is available
  if (uart.peekchar('\n') < 0) return;

  char s[32];
  if (is_scan_mode) {
    // Scan tokens until new-line
    if (trace.scan(s, sizeof(s)) && *s == '\n') return;
    trace << Watchdog::millis() << ':' << n << ':';
    trace << PSTR("echo('") << s << PSTR("')") << endl;
    // Check for mode change command
    if (strcmp_P(s, PSTR("LINE")) == 0) {
      if (trace.scan(s, sizeof(s)) && *s == '\n') {
	is_scan_mode = false;
	return;
      }
      trace << Watchdog::millis() << ':' << n << ':';
      trace << PSTR("echo('") << s << PSTR("')") << endl;
    }
    while (trace.scan(s, sizeof(s)) && *s != '\n') {
      trace << Watchdog::millis() << ':' << n << ':';
      trace << PSTR("echo('") << s << PSTR("')") << endl;
    }
  } 
  else {
    // Scan the line. Skip empty lines
    if (uart.gets(s, sizeof(s)) == NULL) return;
    trace << Watchdog::millis() << ':' << n << ':';
    trace << PSTR("echo('") << s << PSTR("')") << endl;

    // Check for mode change command
    is_scan_mode = strcmp_P(s, PSTR("SCAN")) == 0;
  }
}
