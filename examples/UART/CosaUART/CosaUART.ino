/**
 * @file CosaUART.ino
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
 * Cosa IOStream::Device UART line scan example.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

void echo(uint16_t n, char* s)
{
  trace << Watchdog::millis() << ':' << n << ':';
  trace << PSTR("echo('") << s << PSTR("')") << endl;
}

void setup()
{
  // Start the UART and Watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaUART: started (LINE MODE)"));
  Watchdog::begin();
}

void loop()
{
  // Count the number of wake-ups
  static uint16_t n = 0;
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
    echo(n, s);
    // Check for mode change command
    if (strcmp_P(s, PSTR("LINE")) == 0) {
      if (trace.scan(s, sizeof(s)) && *s == '\n') {
	trace << PSTR("LINE MODE") << endl;
	is_scan_mode = false;
	return;
      }
      echo(n, s);
    }
    while (trace.scan(s, sizeof(s)) && *s != '\n') echo(n, s);
  }
  else {
    // Scan the line. Skip empty lines
    if (uart.gets(s, sizeof(s)) == NULL) return;
    s[strlen(s)-1] = 0;
    echo(n, s);

    // Check for mode change command
    is_scan_mode = strcmp_P(s, PSTR("SCAN")) == 0;
    if (is_scan_mode) trace << PSTR("SCAN MODE") << endl;
  }
}
