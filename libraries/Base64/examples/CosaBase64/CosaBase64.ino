/**
 * @file CosaBase64.ino
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
 * Demonstration of the Cosa Base64 encoder/decoder.
 *
 * @section Circuit
 * No special circuit. Uses UART, RTC/Timer and Watchdog.
 *
 * @section References
 * 1. http://en.wikipedia.org/wiki/Base64
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Base64.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBase64: started"));
  TRACE(free_memory());
  Watchdog::begin();
  RTT::begin();
}

// Data to verify block handling and padding
static const uint8_t data[] = {
  0x87, 0x65, 0x43,
  0x21, 0x00, 0x12,
  0x34, 0x56, 0x78
};

// Classical test string; A quote from Thomas Hobbes' Leviathan
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
  start = RTT::micros();
  n = Base64::encode_P(res, PSTR("Nisse badar"), 12);
  stop = RTT::micros();
  trace << n << PSTR(":encode:") << (stop - start) << PSTR(" us") << endl;
  trace << res << endl;
  trace << endl;
  sleep(1);

  // Decode the encoded string
  start = RTT::micros();
  m = Base64::decode(temp, res, n);
  stop = RTT::micros();
  trace << m << PSTR(":decode:") << (stop - start) << PSTR(" us") << endl;
  trace << (char*) temp << endl;
  trace << endl;
  sleep(1);

  // Encode and decode data from a single byte to the full size of data
  for (uint8_t i = 1; i <= sizeof(data); i++) {
    trace << i << ':';
    trace.print(data, i, IOStream::hex);

    start = RTT::micros();
    n = Base64::encode(res, data, i);
    stop = RTT::micros();
    trace << n << PSTR(":encode:") << (stop - start) << PSTR(" us") << endl;
    trace << res << endl;

    start = RTT::micros();
    m = Base64::decode(temp, res, n);
    stop = RTT::micros();
    trace << m << PSTR(":decode:") << (stop - start) << PSTR(" us") << endl;
    trace.print(temp, m, IOStream::hex);
    trace << endl;
    sleep(1);
  }

  // Encode data directly to the uart iobuffer
  start = RTT::micros();
  n = Base64::encode(&uart, data, sizeof(data));
  stop = RTT::micros();
  trace << endl;
  trace << n << PSTR(":encode:") << (stop - start) << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  // Encode a large string directly to the uart iobuffer. This could be
  // any iostream such as Socket::Driver/Ethernet.
  start = RTT::micros();
  n = Base64::encode_P(&uart, citation, strlen_P(citation));
  stop = RTT::micros();
  trace << endl;
  trace << n << PSTR(":encode:") << (stop - start) << PSTR(" us") << endl;
  trace << endl;
  // Note: the measurement is bound by the UART buffer size
  sleep(1);

  ASSERT(true == false);
}

/**
 * @section Output
 *
 *  CosaBase64: started
 *  free_memory() = 1586
 *  16:encode:32 us
 *  Tmlzc2UgYmFkYXIA
 *
 *  12:decode:52 us
 *  Nisse badar
 *
 *  1:0x118: 87
 *  4:encode:12 us
 *  hw==
 *  3:decode:20 us
 *  0x8da: 87 00 00
 *
 *  2:0x118: 87 65
 *  4:encode:12 us
 *  h2U=
 *  3:decode:20 us
 *  0x8da: 87 65 00
 *
 *  3:0x118: 87 65 43
 *  4:encode:12 us
 *  h2VD
 *  3:decode:20 us
 *  0x8da: 87 65 43
 *
 *  4:0x118: 87 65 43 21
 *  8:encode:16 us
 *  h2VDIQ==
 *  6:decode:28 us
 *  0x8da: 87 65 43 21 00 00
 *
 *  5:0x118: 87 65 43 21 00
 *  8:encode:16 us
 *  h2VDIQA=
 *  6:decode:28 us
 *  0x8da: 87 65 43 21 00 00
 *
 *  6:0x118: 87 65 43 21 00 12
 *  8:encode:20 us
 *  h2VDIQAS
 *  6:decode:32 us
 *  0x8da: 87 65 43 21 00 12
 *
 *  7:0x118: 87 65 43 21 00 12 34
 *  12:encode:24 us
 *  h2VDIQASNA==
 *  9:decode:40 us
 *  0x8da: 87 65 43 21 00 12 34 00 00
 *
 *  8:0x118: 87 65 43 21 00 12 34 56
 *  12:encode:24 us
 *  h2VDIQASNFY=
 *  9:decode:40 us
 *  0x8da: 87 65 43 21 00 12 34 56 00
 *
 *  9:0x118: 87 65 43 21 00 12 34 56 78
 *  12:encode:20 us
 *  h2VDIQASNFZ4
 *  9:decode:44 us
 *  0x8da: 87 65 43 21 00 12 34 56 78
 *
 *  h2VDIQASNFZ4
 *  12:encode:116 us
 *
 *  TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1
 *  dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3
 *  aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFu
 *  Y2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxl
 *  IGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhl
 *  bWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=
 *  360:encode:343240 us
 */
