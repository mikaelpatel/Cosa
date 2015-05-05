/**
 * @file CosaProtocolBuffer.ino
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
 * Cosa ProtocolBuffer demonstration and tests.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <ProtocolBuffer.h>

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Memory.h"

// Selectable test suites
#define TEST_RANGE
#define TEST_STRUCT
#if !defined(BOARD_ATTINY)
# define TEST_LIMITS
# define TEST_EXAMPLES
#endif

void setup()
{
  IOBuffer<32> iob;
  ProtocolBuffer pb(&iob, &iob);
  ProtocolBuffer::Type type;
  char buf[32];
  uint8_t tag;
  uint32_t u;
  int32_t v;

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaProtocolBuffer: started"));
  TRACE(free_memory());
  TRACE(sizeof(iob));
  TRACE(sizeof(pb));

#if defined(TEST_LIMITS)
  // Test unsigned lower limit
  u = 0;
  ASSERT(pb.write(1, u) == 2);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  u = 0;
  ASSERT(pb.read(u) == 1);
  ASSERT(u == 0);

  // Test unsigned upper limit
  u = 0xffffffff;
  ASSERT(pb.write(1, u) == 6);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  u = 0;
  ASSERT(pb.read(u) == 5);
  ASSERT(u == 0xffffffff);

  // Test signed upper limit
  v = 0x7fffffff;
  ASSERT(pb.write(1, v) == 6);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  v = 0;
  ASSERT(pb.read(v) == 5);
  ASSERT(v == 0x7fffffff);

  // Test signed lower limit
  v = 0x80000000;
  ASSERT(pb.write(1, v) == 6);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  v = 0;
  ASSERT(pb.read(v) == 5);
  ASSERT(v == (int32_t) 0x80000000);

  // Test a length delimited string
  static char s[] = "Google Protocol Buffers";
  ASSERT(pb.write(1, s, strlen(s) + 1) == (int) strlen(s) + 3);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::LENGTH_DELIMITED);
  memset(buf, 0, sizeof(buf));
  ASSERT(pb.read(buf, sizeof(buf)) == (int) strlen(s) + 1);
  ASSERT(strcmp(buf,s) == 0);
  trace << buf << endl;

  // Test a floating point number
  float32_t f = -1.53729e-3;
  ASSERT(pb.write(1, f) == (sizeof(f) + 1));
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::FIXED32);
  f = 0.0;
  ASSERT(pb.read(f) == sizeof(f));
  ASSERT(f == -1.53729e-3);
#endif

#if defined(TEST_RANGE)
  // Test signed integer range
  for (int32_t value = -100; value <= 100; value += 10) {
    v = 0;
    ASSERT(pb.write(1, value) > 0);
    ASSERT(pb.read(tag, type) == 1);
    ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
    ASSERT(pb.read(v) > 0);
    ASSERT(v == value);
  }

  // Test unsigned integer range
  for (uint32_t value = 0; value <= 200; value += 10) {
    ASSERT(pb.write(2, value) > 0);
    ASSERT(pb.read(tag, type) == 1);
    ASSERT(tag == 2 && type == ProtocolBuffer::VARINT);
    ASSERT(pb.read(u) > 0);
    ASSERT(u == value);
  }
#endif

#if defined(TEST_STRUCT)
  // Test reading and writing a struct
  struct {
    uint8_t x;
    int16_t y;
    uint32_t z;
  } foo;

  // Assign struct
  foo.x = 10;
  foo.y = -100;
  foo.z = 1000000UL;

  // Write each struct member with a tag
  ASSERT(pb.write(1, foo.x) == 2);
  ASSERT(pb.write(2, foo.y) == 3);
  ASSERT(pb.write(3, foo.z) == 4);

  // Read and verify tag and value
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  ASSERT(pb.read(u) == 1);
  ASSERT(u == 10);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 2 && type == ProtocolBuffer::VARINT);
  ASSERT(pb.read(v) == 2);
  ASSERT(v == -100);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 3 && type == ProtocolBuffer::VARINT);
  ASSERT(pb.read(u) == 3);
  ASSERT(u == 1000000UL);

  // Write the struct as a length delimited value
  ASSERT(pb.write(1, &foo, sizeof(foo)) == (sizeof(foo) + 2));
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::LENGTH_DELIMITED);

  // Read and verify value
  memset(buf, 0, sizeof(buf));
  ASSERT(pb.read(buf, sizeof(buf)) == sizeof(foo));
  ASSERT(memcmp(buf, &foo, sizeof(foo)) == 0);
#endif

#if defined(TEST_EXAMPLES)
  // From Google Developers Protocol Buffers Encoding
  // https://developers.google.com/protocol-buffers/docs/encoding
  //
  // A SIMPLE MESSAGE
  //
  //   message Test1 {
  //     required int32 a = 1;
  //   }
  //
  // In an application, you create a Test1 message and set a to
  // 150. You then serialize the message to an output stream. If you
  // were able to examine the encoded message, you'd see three bytes:
  u = 150;
  ASSERT(pb.write(1, u) == 3);
  trace.print((const char*) iob, iob.available(), IOStream::hex);
  // Result: 08 96 01
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  u = 0;
  ASSERT(pb.read(u) == 2);
  ASSERT(u == 150);
  //
  // MORE VALUE TYPES: SIGNED INTEGER
  //
  // ZigZag encoding maps signed integers to unsigned integers so that
  // numbers with a small absolute value (for instance, -1) have a
  // small varint encoded value too. It does this in a way that
  // "zig-zags" back and forth through the positive and negative
  // integers, so that -1 is encoded as 1, 1 is encoded as 2, -2 is
  // encoded as 3, and so on.
  v = -1;
  ASSERT(pb.write(1, v) == 2);
  trace.print((const char*) iob, iob.available(), IOStream::hex);
  // Result: 08 01
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  v = 0;
  ASSERT(pb.read(v) == 1);
  ASSERT(v == -1);
  //
  v = -2;
  ASSERT(pb.write(1, v) == 2);
  trace.print((const char*) iob, iob.available(), IOStream::hex);
  // Result: 08 03
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  v = 0;
  ASSERT(pb.read(v) == 1);
  ASSERT(v == -2);
  //
  // MORE VALUE TYPES: STRINGS
  //
  // A wire type of 2 (length-delimited) means that the value is a
  // varint encoded length followed by the specified number of bytes
  // of data.
  //
  //   message Test2 {
  //     required string b = 2;
  //   }
  //
  // Setting the value of b to "testing".
  const char b[] = "testing";
  ASSERT(pb.write(2, b) == strlen(b) + 2);
  trace.print((const char*) iob, iob.available(), IOStream::hex);
  // Result: 12 07 74 65 73 74 69 6e 67
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 2 && type == ProtocolBuffer::LENGTH_DELIMITED);
  memset(buf, 0, sizeof(buf));
  ASSERT(pb.read(buf, sizeof(buf)) == (int) strlen(b));
  ASSERT(memcmp(buf, b, strlen(b)) == 0);
  iob.empty();
  //
  // Use a string in program memory.
  static const char b_P[] __PROGMEM = "testing";
  ASSERT(pb.write_P(2, b_P) == (int) strlen_P(b_P) + 2);
  trace.print((const char*) iob, iob.available(), IOStream::hex);
  // Result: 12 07 74 65 73 74 69 6e 67
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 2 && type == ProtocolBuffer::LENGTH_DELIMITED);
  memset(buf, 0, sizeof(buf));
  ASSERT(pb.read(buf, sizeof(buf)) == (int) strlen_P(b_P));
  ASSERT(memcmp_P(buf, b_P, strlen_P(b_P)) == 0);
  iob.empty();
  //
  // EMBEDDED MESSAGES
  //
  // Here's a message definition with an embedded message of our
  // example type, Test1:
  //
  //   message Test3 {
  //     required Test1 c = 3;
  //   }
  //
  // And here's the encoded version, again with the Test1's a field
  // set to 150. Use a temporary buffer for the encoding of the
  // embedded message.
  IOBuffer<32> tmp;
  ProtocolBuffer c(&tmp, &tmp);
  int count;
  u = 150;
  ASSERT(c.write(1, u) == 3);
  ASSERT(pb.write(3, (const char*) tmp, tmp.available()) ==
	 tmp.available() + 2);
  trace.print((const char*) iob, iob.available(), IOStream::hex);
  // Result: 1a 03 08 96 01
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 3 && type == ProtocolBuffer::LENGTH_DELIMITED);
  ASSERT((count = pb.getchar()) == 3);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::VARINT);
  u = 0;
  ASSERT(pb.read(u) == 2);
  ASSERT(u == 150);
  tmp.empty();
  //
  // REPEATED ELEMENTS
  //
  // Version 2.1.0 introduced packed repeated fields, which are
  // declared like repeated fields but with the special [packed=true]
  // option. These function like repeated fields, but are encoded
  // differently. A packed repeated field containing zero elements
  // does not appear in the encoded message. Otherwise, all of the
  // elements of the field are packed into a single key-value pair
  // with wire type 2 (length-delimited). Each element is encoded the
  // same way it would be normally, except without a tag preceding it.
  //
  //   message Test4 {
  //     repeated int32 d = 4 [packed=true];
  //   }
  //
  // Now let's say you construct a Test4, providing the values 3, 270,
  // and 86942 for the repeated field d.
  ASSERT(c.write(3UL) == 1);
  ASSERT(c.write(270UL) == 2);
  ASSERT(c.write(86942UL) == 3);
  ASSERT(pb.write(4, (const char*) tmp, tmp.available()) ==
	 tmp.available() + 2);
  trace.print((const char*) iob, iob.available(), IOStream::hex);
  // Result: 22 06 03 8e 02 9e a7 05
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 4 && type == ProtocolBuffer::LENGTH_DELIMITED);
  ASSERT((count = pb.getchar()) == 6);
  ASSERT(pb.read(u) == 1);
  ASSERT(u == 3UL);
  ASSERT(pb.read(u) == 2);
  ASSERT(u == 270UL);
  ASSERT(pb.read(u) == 3);
  ASSERT(u == 86942UL);
#endif
}

void loop()
{
  TRACE(free_memory());
  ASSERT(true == false);
}


/**
 * @section Output
 *  CosaProtocolBuffer: started
 *  free_memory() = 1540
 *  sizeof(iob) = 37
 *  sizeof(pb) = 4
 *  Google Protocol Buffers
 *  0x895: 08 96 01
 *  0x898: 08 01
 *  0x89a: 08 03
 *  0x89c: 12 07 74 65 73 74 69 6e 67
 *  0x88c: 12 07 74 65 73 74 69 6e 67
 *  0x88c: 1a 03 08 96 01
 *  0x891: 22 06 03 8e 02 9e a7 05
 *  free_memory() = 1695
 *  334:void loop():assert:true == false
 */
