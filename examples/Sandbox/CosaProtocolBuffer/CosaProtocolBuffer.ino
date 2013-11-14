/**
 * @file CosaProtocolBuffer.ino
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
 * Cosa ProtocolBuffer demonstration and tests.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOBuffer.hh"
#include "Cosa/ProtocolBuffer.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

#define TEST_RANGE
#define TEST_LIMITS
#define TEST_STRUCT

IOBuffer<32> buf;
ProtocolBuffer pb(&buf, &buf);

void setup()
{
  static char s[] = "Google Protocol Buffers";
  ProtocolBuffer::Type type;
  char buf[32];
  uint8_t tag;
  float32_t f;
  uint32_t u;
  int32_t v;

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaProtocolBuffer: started"));
  TRACE(sizeof(buf));
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
  ASSERT(pb.write(1, s, strlen(s) + 1) == (int) strlen(s) + 3);
  ASSERT(pb.read(tag, type) == 1);
  ASSERT(tag == 1 && type == ProtocolBuffer::LENGTH_DELIMITED);
  memset(buf, 0, sizeof(buf));
  ASSERT(pb.read(buf, sizeof(buf)) == (int) strlen(s) + 1);
  ASSERT(strcmp(buf,s) == 0);
  trace << buf << endl;

  // Test a floating point number
  f = -1.53729e-3;
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
}

void loop()
{
  ASSERT(true == false);
}

