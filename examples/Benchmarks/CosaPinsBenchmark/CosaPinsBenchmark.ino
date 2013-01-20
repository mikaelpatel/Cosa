/**
 * @file CosaPinsBenchmark.ino
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
 * Cosa Pins Benchmark; number of micro-seconds for pin operations.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/UART.hh"

InputPin inPin(Board::D7);
OutputPin outPin(Board::D8);
OutputPin dataPin(Board::D9);
OutputPin clockPin(Board::D10);
AnalogPin analogPin(Board::A0);

void setup()
{
  uint32_t start, stop;

  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinsBenchmark: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(Event::Handler));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(AnalogPin));

  // Start the timers
  RTC::begin();
  Watchdog::begin();

  // Measure the time to perform 1,000,000 input pin reads
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      digitalRead(7);
    }
  stop = RTC::micros();
  INFO("Arduino: %ul us per 1000 digitalRead(7)", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      inPin.is_set();
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 inPin.is_set()", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      InputPin::read(7);
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 InputPin::read(7)\n", (stop - start) / 1000L);


  // Measure the time to perform 1,000,000 output pin writes
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      digitalWrite(8, 1);
      digitalWrite(8, 0);
    }
  stop = RTC::micros();
  INFO("Arduino: %ul us per 1000 digitalWrite(8, 1); digitalWrite(8, 0)", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.write(1);
      outPin.write(0);
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 outPin.write(1); outPin.write(0)", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.set();
      outPin.clear();
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 outPin.set; outPin.clear()", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::write(8, 1);
      OutputPin::write(8, 0);
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 OutputPin::write(8, 1); OutputPin::write(8, 0)\n", (stop - start) / 1000L);

  // Measure the time to perform 1,000,000 output pin toggle
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      digitalWrite(8, !digitalRead(8));
    }
  stop = RTC::micros();
  INFO("Arduino: %ul us per 1000 digitalWrite(8, !digitalRead(8))", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.write(!outPin.read());
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 outPin.write(!outPin.read())", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (outPin.is_set()) outPin.clear(); else outPin.set();
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 outPin.is_set/clear/set()", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.set(outPin.is_clear());
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 outPin.set/is_clear()", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.toggle();
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 outPin.toggle()", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::write(8, !OutputPin::read(8));
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 OutputPin::write(8, !OutputPin::read(8))", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (OutputPin::read(8))
	OutputPin::write(8, 0);
      else
	OutputPin::write(8, 1);
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 OutputPin::read/write(8,0/1)", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::toggle(8);
    }
  stop = RTC::micros();
  INFO("Cosa: %ul us per 1000 OutputPin::toggle(8)\n", (stop - start) / 1000L);

  // Measure the time to perform 1,000 byte data transfer
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      digitalWrite(9, data & bit);
      digitalWrite(10, 1);
      digitalWrite(10, 0);
    }
  }
  stop = RTC::micros();
  INFO("Arduino: %ul us per bit data transfer() digitalWrite()", (stop - start) / 8000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      clockPin.write(1);
      clockPin.write(0);
    }
  }
  stop = RTC::micros();
  INFO("Cosa: %ul us per bit data transfer() pin.write()", (stop - start) / 8000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      clockPin.toggle();
      clockPin.toggle();
    }
  }
  stop = RTC::micros();
  INFO("Cosa: %ul us per bit data transfer() pin.write/toggle()", (stop - start) / 8000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(9, data & bit);
      OutputPin::write(10, 1);
      OutputPin::write(10, 0);
    }
  }
  stop = RTC::micros();
  INFO("Cosa: %ul us per bit data transfer() OutputPin::write()", (stop - start) / 8000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(9, data & bit);
      OutputPin::toggle(10);
      OutputPin::toggle(10);
    }
  }
  stop = RTC::micros();
  INFO("Cosa: %ul us per bit data transfer() OutputPin::write/toggle()", (stop - start) / 8000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    dataPin.write(data & 0x80);
    clockPin.toggle();
    clockPin.toggle();
    dataPin.write(data & 0x40);
    clockPin.toggle();
    clockPin.toggle();
    dataPin.write(data & 0x20);
    clockPin.toggle();
    clockPin.toggle();
    dataPin.write(data & 0x10);
    clockPin.toggle();
    clockPin.toggle();
    dataPin.write(data & 0x08);
    clockPin.toggle();
    clockPin.toggle();
    dataPin.write(data & 0x04);
    clockPin.toggle();
    clockPin.toggle();
    dataPin.write(data & 0x02);
    clockPin.toggle();
    clockPin.toggle();
    dataPin.write(data & 0x01);
    clockPin.toggle();
    clockPin.toggle();
  }
  stop = RTC::micros();
  INFO("Cosa: %ul us per bit data transfer() pin.write/toggle() unrolled\n", (stop - start) / 8000L);
  
  // Measure the time to perform 1,000 analog pin samples
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    analogRead(0);
  stop = RTC::micros();
  INFO("Arduino: %ul us per analogRead()", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    analogPin.sample();
  stop = RTC::micros();
  INFO("Cosa: %ul us per analogPin.sample()", (stop - start) / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    AnalogPin::sample(0);
  stop = RTC::micros();
  INFO("Cosa: %ul us per AnalogPin::sample()", (stop - start) / 1000L);
}

void loop()
{
}
