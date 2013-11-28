/**
 * @file CosaBase64.ino
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Demonstration of the Cosa Base64 encoder/decoder; This is a  
 * non standard Base64 algorithm that produces printable characters
 * in a given ASCII range instead of the RFC1113 standard.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Cipher/Base64.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBase64: started"));
  TRACE(free_memory());
  Watchdog::begin();
  RTC::begin();
}

// Data to verify block handling and padding
static const uint8_t data[] = { 
  0x87, 0x65, 0x43, 
  0x21, 0x00, 0x12, 
  0x34, 0x56, 0x78 
};

// Classical test string
static const char citation[] __PROGMEM = 
  "Man is distinguished, not only by his reason, but by this singular "
  "passion from other animals, which is a lust of the mind, that by a "
  "perseverance of delight in the continued and indefatigable generation "
  "of knowledge, exceeds the short vehemence of any carnal pleasure.";

void loop()
{
  uint32_t start, stop;
  uint8_t temp[32];
  char res[32];
  int n, m;
  
  // Encode a string in program memory
  start = RTC::micros();
  n = Base64::encode_P(res, PSTR("Nisse badar"), 12);
  stop = RTC::micros();
  trace << n << PSTR(":encode:ms = ") << (stop - start) << PSTR(" us") << endl;
  trace << res << endl;
  trace << endl;
  SLEEP(1);

  // Decode the encoded string
  start = RTC::micros();
  m = Base64::decode(temp, res, n);
  stop = RTC::micros();
  trace << m << PSTR(":decode:ms = ") << (stop - start) << PSTR(" us") << endl;
  trace << (char*) temp << endl;
  trace << endl;
  SLEEP(1);

  // Encode and decode data from a single byte to the full size of data
  for (uint8_t i = 1; i <= sizeof(data); i++) {
    trace << i << ':'; 
    trace.print(data, i, IOStream::hex);

    start = RTC::micros();
    n = Base64::encode(res, data, i);
    stop = RTC::micros();
    trace << n << PSTR(":encode:ms = ") << (stop - start) << PSTR(" us") << endl;
    trace << res << endl;

    start = RTC::micros();
    m = Base64::decode(temp, res, n);
    stop = RTC::micros();
    trace << m << PSTR(":decode:ms = ") << (stop - start) << PSTR(" us") << endl;
    trace.print(temp, m, IOStream::hex);
    trace << endl;
    SLEEP(1);
  }

  // Encode data directly to the uart iobuffer
  start = RTC::micros();
  n = Base64::encode(&uart, data, sizeof(data));
  stop = RTC::micros();
  trace << endl;
  trace << n << PSTR(":encode:ms = ") << (stop - start) << PSTR(" us") << endl;
  trace << endl;
  SLEEP(1);

  // Encode a large string directly to the uart iobuffer
  start = RTC::micros();
  n = Base64::encode_P(&uart, citation, strlen_P(citation));
  stop = RTC::micros();
  trace << endl;
  trace << n << PSTR(":encode:ms = ") << (stop - start) << PSTR(" us") << endl;
  trace << endl;
  SLEEP(1);

  ASSERT(true == false);
}
