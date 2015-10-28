/**
 * @file CosaBenchmarkSizeOf.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Cosa sizeof benchmark; the size of class instances. Static member
 * data are not included. Note that this is also a list of the classes
 * available in Cosa.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#define TRACE_NO_VERBOSE

#include "Cosa/Memory.h"

#include "Cosa/Alarm.hh"
#include "Cosa/AnalogComparator.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/AnalogPins.hh"
#include "Cosa/BitSet.hh"
#include "Cosa/Button.hh"
#include "Cosa/Clock.hh"
#include "Cosa/EEPROM.hh"
#include "Cosa/Event.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Flash.hh"
#include "Cosa/FSM.hh"
#include "Cosa/GPIO.hh"
#include "Cosa/InputCapture.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/INET.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/IOEvent.hh"
#include "Cosa/IOPin.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/CDC.hh"
#include "Cosa/UART.hh"
#include "Cosa/WIO.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/Job.hh"
#include "Cosa/Keypad.hh"
#include "Cosa/LCD.hh"
#include "Cosa/LED.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Listener.hh"
#include "Cosa/Lock.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Pin.hh"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/PWMPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Queue.hh"
#include "Cosa/Resource.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Socket.hh"
#include "Cosa/SPI.hh"
#include "Cosa/String.hh"
#include "Cosa/Time.hh"
#include "Cosa/Trace.hh"
#include "Cosa/TWI.hh"
#include "Cosa/Wireless.hh"

void setup()
{
  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkSizeOf: started"));

  // Print size of instance
  TRACE(sizeof(Alarm));
  TRACE(sizeof(Alarm::Clock));
  TRACE(sizeof(AnalogComparator));
  TRACE(sizeof(AnalogPin));
  TRACE(sizeof(AnalogPins));
  TRACE(sizeof(BitSet<64>));
  TRACE(sizeof(Button));
#if defined(USBCON)
  TRACE(sizeof(CDC));
#endif
  TRACE(sizeof(Clock));
  TRACE(sizeof(EEPROM));
  TRACE(sizeof(EEPROM::Device));
  TRACE(sizeof(Event));
  TRACE(sizeof(Event::Handler));
  TRACE(sizeof(Event::queue));
  TRACE(sizeof(ExternalInterrupt));
  TRACE(sizeof(Flash));
  TRACE(sizeof(Flash::Device));
  TRACE(sizeof(FSM));
  TRACE(sizeof(GPIO));
  TRACE(sizeof(InputCapture));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(Interrupt::Handler));
  TRACE(sizeof(IOBuffer<64>));
  TRACE(sizeof(IOEvent<UART>));
  TRACE(sizeof(IOPin));
  TRACE(sizeof(IOStream));
  TRACE(sizeof(IOStream::Device));
  TRACE(sizeof(Job));
  TRACE(sizeof(Job::Scheduler));
#if !defined(BOARD_ATTINY)
  TRACE(sizeof(UART));
#endif
  TRACE(sizeof(Keypad));
  TRACE(sizeof(LCD));
  TRACE(sizeof(LCD::Device));
  TRACE(sizeof(LCD::IO));
  TRACE(sizeof(LCD::Serial3W));
  TRACE(sizeof(LCD::SPI3W));
  TRACE(sizeof(LCD::Keypad));
  TRACE(sizeof(LED));
  TRACE(sizeof(Linkage));
  TRACE(sizeof(Link));
  TRACE(sizeof(Head));
  TRACE(sizeof(Listener<int>));
  TRACE(sizeof(Lock));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(Periodic));
  TRACE(sizeof(Pin));
  TRACE(sizeof(PWMPin));
  TRACE(sizeof(Queue<Event,16>));
  TRACE(sizeof(Resource));
  TRACE(sizeof(RTT));
  TRACE(sizeof(RTT::Scheduler));
  TRACE(sizeof(RTT::Clock));
  TRACE(sizeof(Serial));
  TRACE(sizeof(Socket));
  TRACE(sizeof(SPI::Driver));
  TRACE(sizeof(String));
  TRACE(sizeof(clock_t));
  TRACE(sizeof(time_t));
  TRACE(sizeof(Trace));
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(TWI::Slave));
  TRACE(sizeof(Watchdog));
  TRACE(sizeof(Watchdog::Scheduler));
  TRACE(sizeof(Watchdog::Clock));
  TRACE(sizeof(Wireless::Driver));
}

void loop()
{
  ASSERT(true == false);
}
