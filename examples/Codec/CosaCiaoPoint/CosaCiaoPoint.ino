/**
 * @file CosaCiaoPoint.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Demonstration of Cosa Ciao data streaming format; type description
 * and streaming an user defined data type. Open the serial monitor to
 * see the print out of the serialized data stream. No special host
 * program is required, the example sketch will print the stream as
 * hexadecimal numbers and printable characters when possible.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#include "Cosa/Serializer/Ciao.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

#include <ctype.h>

// Ciao output stream
Ciao cout;

// A Point data type
struct Point {
  int16_t x;
  int16_t y;
};

// Ciao data type descriptor in program memory for Point
const uint16_t Point_ID = 0x1042;
const char Point_name[] __PROGMEM = "::Point";
const char Point_x_name[] __PROGMEM = "x";
const char Point_y_name[] __PROGMEM = "y";
const Ciao::Descriptor::member_t Point_members[] __PROGMEM = {
  {
    Ciao::INT16_TYPE,
    1,
    Point_x_name,
    0
  },
  {
    Ciao::INT16_TYPE,
    1,
    Point_y_name,
    0
  }
};
const Ciao::Descriptor::user_t Point_desc __PROGMEM = {
  Point_ID,
  Point_name,
  Point_members,
  membersof(Point_members)
};

// We need a trick to allow mapping the binary stream to textual trace
// This is basically an example of the Decorator Design Pattern
// Otherwise we would need to write a host program
class TraceDevice : public IOStream::Device {
public:
  virtual int putchar(char c)
  {
    trace.print((uint8_t) c, IOStream::hex);
    if (isgraph(c)) {
      trace.print(PSTR(" '"));
      trace.print(c);
      trace.print('\'');
    }
    trace.println();
    return (1);
  }
};

// The new IOStream device that will print in hexadecimal
TraceDevice traceDevice;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCiaoPoint: started"));

  // Setup and start the data output stream on the trace device
  cout.set(&traceDevice);

  INFO("Write the header to the trace device", 0);
  cout.begin();

  INFO("Stream Ciao header descriptor", 0);
  cout.write(&Ciao::Descriptor::header_t);

  INFO("Stream Point type descriptor", 0);
  cout.write(&Point_desc);

  INFO("Stream Point values with user type prefix", 0);
  Point p = { -1, 1 };
  cout.write(&Point_desc, &p, 1);

  Point q[] = { { -100, -100 }, { 100, 100 } };
  cout.write(&Point_desc, &q, membersof(q));
}

void loop()
{
}

/**
 * @section Output
 *  CosaCiaoPoint: started
 *  109:void setup():info:Write the header to the trace device
 *  0x61 'a'
 *  0x0
 *  0x43 'C'
 *  0x6f 'o'
 *  0x73 's'
 *  0x61 'a'
 *  0x3a ':'
 *  0x3a ':'
 *  0x43 'C'
 *  0x69 'i'
 *  0x61 'a'
 *  0x6f 'o'
 *  0x0
 *  0x1
 *  0x0
 *  0x0
 *  112:void setup():info:Stream Ciao header descriptor
 *  0x40 '@'
 *  0x0
 *  0x43 'C'
 *  0x69 'i'
 *  0x61 'a'
 *  0x6f 'o'
 *  0x3a ':'
 *  0x3a ':'
 *  0x68 'h'
 *  0x65 'e'
 *  0x61 'a'
 *  0x64 'd'
 *  0x65 'e'
 *  0x72 'r'
 *  0x5f '_'
 *  0x74 't'
 *  0x0
 *  0x0
 *  0x6d 'm'
 *  0x61 'a'
 *  0x67 'g'
 *  0x69 'i'
 *  0x63 'c'
 *  0x0
 *  0x1
 *  0x6d 'm'
 *  0x61 'a'
 *  0x6a 'j'
 *  0x6f 'o'
 *  0x72 'r'
 *  0x0
 *  0x1
 *  0x6d 'm'
 *  0x69 'i'
 *  0x6e 'n'
 *  0x6f 'o'
 *  0x72 'r'
 *  0x0
 *  0x1
 *  0x65 'e'
 *  0x6e 'n'
 *  0x64 'd'
 *  0x69 'i'
 *  0x61 'a'
 *  0x6e 'n'
 *  0x0
 *  0x4f 'O'
 *  115:void setup():info:Stream Point type descriptor
 *  0x50 'P'
 *  0x10
 *  0x42 'B'
 *  0x3a ':'
 *  0x3a ':'
 *  0x50 'P'
 *  0x6f 'o'
 *  0x69 'i'
 *  0x6e 'n'
 *  0x74 't'
 *  0x0
 *  0x91
 *  0x78 'x'
 *  0x0
 *  0x91
 *  0x79 'y'
 *  0x0
 *  0x5f '_'
 *  118:void setup():info:Stream Point values with user type prefix
 *  0x71 'q'
 *  0x10
 *  0x42 'B'
 *  0xff
 *  0xff
 *  0x1
 *  0x0
 *  0x72 'r'
 *  0x10
 *  0x42 'B'
 *  0x9c
 *  0xff
 *  0x9c
 *  0xff
 *  0x64 'd'
 *  0x0
 *  0x64 'd'
 *  0x0
 */
