/**
 * @file CosaCiaoPoint.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Example program for the Ciao streaming format; declaration and
 * streaming of a data type.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Ciao.h"
#include "Cosa/IOStream.h"
#include "Cosa/Trace.h"

// Ciao output stream over the UART
Ciao outStream;

// A data type to stream
struct Point {
  int16_t x;
  int16_t y;
};

// Ciao data type declaraction in program memory
const char Point_x_name[] PROGMEM = "x";
const char Point_y_name[] PROGMEM = "y";
const Ciao::decl_member_t Point_member[] PROGMEM = {
  {
    Ciao::INT16_TYPE,		// type
    0,				// id
    1,				// count
    Point_x_name		// name
  },
  {
    Ciao::INT16_TYPE,		// type
    0,				// id
    1,				// count
    Point_y_name		// name
  }
};
const char Point_name[] PROGMEM = "Point";
const Ciao::decl_user_t Point_decl PROGMEM = {
  0x42,				// id
  Point_name,			// name
  sizeof(Point),		// size
  Point_member,			// member
  membersof(Point_member)	// count
};  

// Arduino build includes stdio so we need to undef
#undef putchar

// This is a dummy device that maps binary streams to the trace iostream
class TraceDevice : public IOStream::Device {
public:
  virtual int putchar(char c);
};

int
TraceDevice::putchar(char c)
{ 
  trace.print((uint8_t) c, 16); 
  trace.println();
  return (1); 
}

// This is what we bind to the Ciao data stream
TraceDevice traceDevice;

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaCiaoPoint: started"));

  // Set the trace device as output stream
  outStream.set(&traceDevice);

  // Value to stream
  Point p = { -1, 1 };

  // Stream raw values first and then the same with type declaration
  outStream.write(p.x);
  outStream.write(p.y);

  // Stream the type declaration
  outStream.write(&Point_decl);

  // Stream the value
  outStream.write(&Point_decl, &p, 1);
}

void loop()
{
}
