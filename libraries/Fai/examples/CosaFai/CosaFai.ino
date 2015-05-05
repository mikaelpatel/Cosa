/**
 * @file CosaFai.ino
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
 * Demonstration of Cosa Ciao data streaming; Cosa Fai, Ardino
 * objects; Writes Cosa Fai header and type descriptor followed by
 * analog and digital pin samples and events.
 *
 * Open the serial monitor to see the print out of the serialized data
 * stream. No special host program is required, the example sketch
 * will print the stream as hexadecimal numbers and printable
 * characters when possible.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output,
 * samples digital pin D2 and analog pin A0.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Ciao.h>
#include <Fai.h>

#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

#include <ctype.h>

// Fai::Ciao output stream over the UART
Fai cout;

// Pins to sample and stream
AnalogPin levelPin(Board::A0);
InputPin onoffPin(Board::D2);
OutputPin ledPin(Board::LED);

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
  trace.begin(&uart, PSTR("CosaFai: started"));

  // Setup and start the data output stream on the trace device
  cout.set(&traceDevice);

  INFO("Write the header to the trace device", 0);
  cout.begin();

  INFO("Write the cosa fai type descriptors", 0);
  cout.Ciao::write(&Fai::Descriptor::analog_pin_t);
  cout.Ciao::write(&Fai::Descriptor::digital_pin_t);
  cout.Ciao::write(&Fai::Descriptor::digital_pins_t);
  cout.Ciao::write(&Fai::Descriptor::event_t);

  // Start the watchdog and trace events
  Watchdog::begin(2048, Watchdog::push_watchdog_event, &ledPin);
}

void loop()
{
  INFO("Wait for timeout event", 0);
  Event event;
  Event::queue.await(&event);

  ledPin.toggle();
  levelPin.sample();

  cout.write(&levelPin);
  cout.write(&onoffPin);
  cout.write(1 << onoffPin.get_pin());
  cout.write(&event);

  ledPin.toggle();
}

/**
 * @section Output
 *  CosaFai: started
 *  82:void setup():info:Write the header to the trace device
 *  0x61 'a'
 *  0x0
 *  0x43 'C'
 *  0x6f 'o'
 *  0x73 's'
 *  0x61 'a'
 *  0x3a ':'
 *  0x3a ':'
 *  0x46 'F'
 *  0x61 'a'
 *  0x69 'i'
 *  0x0
 *  0x1
 *  0x0
 *  0x0
 *  85:void setup():info:Write the cosa fai type descriptors
 *  0x40 '@'
 *  0x10
 *  0x43 'C'
 *  0x6f 'o'
 *  0x73 's'
 *  0x61 'a'
 *  0x3a ':'
 *  0x3a ':'
 *  0x46 'F'
 *  0x61 'a'
 *  0x69 'i'
 *  0x3a ':'
 *  0x3a ':'
 *  0x61 'a'
 *  0x6e 'n'
 *  0x61 'a'
 *  0x6c 'l'
 *  0x6f 'o'
 *  0x67 'g'
 *  0x5f '_'
 *  0x70 'p'
 *  0x69 'i'
 *  0x6e 'n'
 *  0x5f '_'
 *  0x74 't'
 *  0x0
 *  0x1
 *  0x70 'p'
 *  0x69 'i'
 *  0x6e 'n'
 *  0x0
 *  0x11
 *  0x76 'v'
 *  0x61 'a'
 *  0x6c 'l'
 *  0x75 'u'
 *  0x65 'e'
 *  0x0
 *  0x4f 'O'
 *  0x40 '@'
 *  0x11
 *  0x43 'C'
 *  0x69 'i'
 *  0x61 'a'
 *  0x6f 'o'
 *  0x3a ':'
 *  0x3a ':'
 *  0x46 'F'
 *  0x61 'a'
 *  0x69 'i'
 *  0x3a ':'
 *  0x3a ':'
 *  0x64 'd'
 *  0x69 'i'
 *  0x67 'g'
 *  0x69 'i'
 *  0x74 't'
 *  0x61 'a'
 *  0x6c 'l'
 *  0x5f '_'
 *  0x70 'p'
 *  0x69 'i'
 *  0x6e 'n'
 *  0x5f '_'
 *  0x74 't'
 *  0x0
 *  0x1
 *  0x70 'p'
 *  0x69 'i'
 *  0x6e 'n'
 *  0x0
 *  0x1
 *  0x76 'v'
 *  0x61 'a'
 *  0x6c 'l'
 *  0x75 'u'
 *  0x65 'e'
 *  0x0
 *  0x4f 'O'
 *  0x40 '@'
 *  0x12
 *  0x43 'C'
 *  0x6f 'o'
 *  0x73 's'
 *  0x61 'a'
 *  0x3a ':'
 *  0x3a ':'
 *  0x46 'F'
 *  0x61 'a'
 *  0x69 'i'
 *  0x3a ':'
 *  0x3a ':'
 *  0x64 'd'
 *  0x69 'i'
 *  0x67 'g'
 *  0x69 'i'
 *  0x74 't'
 *  0x61 'a'
 *  0x6c 'l'
 *  0x5f '_'
 *  0x70 'p'
 *  0x69 'i'
 *  0x6e 'n'
 *  0x73 's'
 *  0x5f '_'
 *  0x74 't'
 *  0x0
 *  0x21 '!'
 *  0x76 'v'
 *  0x61 'a'
 *  0x6c 'l'
 *  0x75 'u'
 *  0x65 'e'
 *  0x73 's'
 *  0x0
 *  0x4f 'O'
 *  0x40 '@'
 *  0x13
 *  0x43 'C'
 *  0x6f 'o'
 *  0x73 's'
 *  0x61 'a'
 *  0x3a ':'
 *  0x3a ':'
 *  0x45 'E'
 *  0x76 'v'
 *  0x65 'e'
 *  0x6e 'n'
 *  0x74 't'
 *  0x0
 *  0x1
 *  0x74 't'
 *  0x79 'y'
 *  0x70 'p'
 *  0x65 'e'
 *  0x0
 *  0x11
 *  0x74 't'
 *  0x61 'a'
 *  0x72 'r'
 *  0x67 'g'
 *  0x65 'e'
 *  0x74 't'
 *  0x0
 *  0x11
 *  0x76 'v'
 *  0x61 'a'
 *  0x6c 'l'
 *  0x75 'u'
 *  0x65 'e'
 *  0x0
 *  0x4f 'O'
 *  97:void loop():info:Wait for timeout event
 *  0x61 'a'
 *  0x10
 *  0xe
 *  0xb0
 *  0x1
 *  0x61 'a'
 *  0x11
 *  0x2
 *  0x0
 *  0x61 'a'
 *  0x12
 *  0x0
 *  0x0
 *  0x0
 *  0x0
 *  0x61 'a'
 *  0x13
 *  0x6
 *  0x0
 *  0x0
 *  0xaf
 *  0x1
 *  97:void loop():info:Wait for timeout event
 */
