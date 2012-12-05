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
Ciao cout;

// A Point data type 
struct Point {
  int16_t x;
  int16_t y;
};

// Ciao data type declaration in program memory for Point
const char Point_x_name[] PROGMEM = "x";
const char Point_y_name[] PROGMEM = "y";
const Ciao::decl_member_t Point_member[] PROGMEM = {
  {
    Ciao::INT16_TYPE,		// type
    1,				// count
    Point_x_name,		// name
    0				// decl
  },
  {
    Ciao::INT16_TYPE,		// type
    1,				// count
    Point_y_name,		// name
    0				// decl
  }
};
const char Point_name[] PROGMEM = "Point";
const Ciao::decl_user_t Point_decl PROGMEM = {
  0x42,				// id
  Point_name,			// name
  Point_member,			// member
  membersof(Point_member)	// count
};  

// Arduino build includes stdio and putchar macro so we need to undef
#undef putchar

// We need a trick to allow mapping the binary stream to textual trace
// Otherwise we would need to write a host program for this
class TraceDevice : public IOStream::Device {
public:
  virtual int putchar(char c)
  { 
    trace.print((uint8_t) c, 16); 
    trace.println();
    return (1); 
  }
};

// The trick IOStream device
TraceDevice traceDevice;

void setup()
{
  // Start trace coutput stream
  trace.begin(9600, PSTR("CosaCiaoPoint: started"));

  // Setup and start the data output stream on the trace device
  cout.set(&traceDevice);
  cout.begin();

  // Values to stream
  char* s = "Ciao!";
  uint8_t x = 15;
  int32_t y = -2;
  int16_t z[] = { 1, 2, 3, 4 };
  float r = 3.14;
  float c[] = { -1.0, 1.0 };

  // Stream raw values first and then the same with type declaration
  cout.write(s);
  cout.write(x);
  cout.write(y);
  cout.write(z, membersof(z));
  cout.write(r);
  cout.write(c, membersof(c));

  // Stream the type declaration
  cout.write(&Point_decl);

  // Stream the value
  Point p = { -1, 1 };
  cout.write(&Point_decl, &p, 1);
  Point q[] = { { -100, -100 }, { 100, 100 } };
  cout.write(&Point_decl, &q, membersof(q));
}

void loop()
{
}
