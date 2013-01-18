/**
 * @file CosaFai.ino
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
 * Example program for the Cosa Fai streaming format.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Fai.hh"
#include "Cosa/Pins.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include <ctype.h>

// Fai::Ciao output stream over the UART
Fai cout;

// Pins to sample and stream
AnalogPin levelPin(Board::A0);
InputPin onoffPin(Board::D2);
OutputPin ledPin(Board::LED);

// Arduino build includes stdio and putchar macro so we need to undef
#undef putchar

// We need a trick to allow mapping the binary stream to textual trace
// This is basically an example of the Decorator Design Pattern
// Otherwise we would need to write a host program
class TraceDevice : public IOStream::Device {
public:
  virtual int putchar(char c)
  { 
    trace.print((uint8_t) c, 16); 
    if (isgraph(c)) {
      trace.print_P(PSTR(" '"));
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
  // Start trace coutput stream
  trace.begin(9600, PSTR("CosaFai: started"));

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
  Watchdog::begin(2048, SLEEP_MODE_IDLE, Watchdog::push_watchdog_event, 
		  &ledPin);
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
