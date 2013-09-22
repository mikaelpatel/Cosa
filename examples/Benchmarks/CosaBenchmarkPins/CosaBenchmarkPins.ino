/**
 * @file CosaBenchmarkPins.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * @section Circuit
 * This example requires no special circuit. Uses serial output,
 * and pins D7-D9 and A0.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

InputPin inPin(Board::D7);
OutputPin outPin(Board::D8);
OutputPin dataPin(Board::D9);
OutputPin clockPin(Board::D10);
AnalogPin analogPin(Board::A0);

// Simple adaptation of the Arduino/Wiring API

inline void digitalWrite(uint8_t pin, uint8_t value)
{
  OutputPin::write(pin, value);
}

inline int digitalRead(uint8_t pin) 
{
  return (InputPin::read(pin));
}

inline int analogRead(uint8_t pin)
{
  return (AnalogPin::sample(pin));
}

inline void pinMode(uint8_t pin, uint8_t mode)
{
  IOPin::set_mode(pin, (IOPin::Mode) mode);
}

void setup()
{
  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkPins: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(Event::Handler));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(AnalogPin));
  
  // Print CPU clock and instructions per 1MHZ 
  TRACE(F_CPU);
  TRACE(I_CPU);
  trace << endl;

  // Start the timers
  RTC::begin();
  Watchdog::begin();
}

void loop()
{
  uint32_t start, stop;
  uint32_t ns;
  uint8_t cnt;

  // Measure the time to perform 1,000,000; empty loop block
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("nop:%ul ns\n", ns);

  // Measure the time to perform 1,000,000 input pin reads
  cnt = 0;
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (inPin.is_set()) cnt++; else cnt--;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read pin:inPin.is_set():%ul ns", ns);
  if (cnt == 0) TRACE(cnt);

  cnt = 0;
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      uint8_t var;
      inPin >> var;
      if (var) cnt++; else cnt--;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read pin:inPin >> var:%ul ns", ns);
  if (cnt == 0) TRACE(cnt);

  cnt = 0;
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (InputPin::read(7)) cnt++; else cnt--;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read pin:InputPin::read(7):%ul ns", ns);
  if (cnt == 0) TRACE(cnt);

  cnt = 0;
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (digitalRead(7)) cnt++; else cnt--;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read digitalRead(7):%ul ns\n", ns);
  if (cnt == 0) TRACE(cnt);

  // Measure the time to perform 1,000,000 output pin writes
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.write(1);
      outPin.write(0);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write pin:outPin.write(1); outPin.write(0):%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.set();
      outPin.clear();
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write pin:outPin.set; outPin.clear():%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin << 1;
      outPin << 0;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write pin:outPin << 1; outPin << 0:%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::write(8, 1);
      OutputPin::write(8, 0);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write pin:OutputPin::write(8, 1); OutputPin::write(8, 0):%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      digitalWrite(8, 1);
      digitalWrite(8, 0);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write pin:digitalWrite(8, 1); OutputPin::write(8, 0):%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.toggle();
      outPin.toggle();
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write pin:outPin.toggle():%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::toggle(8);
      OutputPin::toggle(8);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write pin:OutputPin::toggle(8):%ul ns\n", ns);
  
  // Measure the time to perform 1,000,000 output pin toggle
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.write(!inPin.read());
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read-write pin:outPin.write(!inPin.read()):%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (inPin.is_set()) outPin.clear(); else outPin.set();
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read-write pin:inPin.is_set/outPin.clear/set():%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      uint8_t var;
      inPin >> var;
      outPin << !var;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read-write pin:inPin >> var; outPin << !var:%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.set(inPin.is_clear());
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read-write pin:outPin.set(inPin.is_clear()):%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::write(8, !InputPin::read(7));
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read-write pin:OutputPin::write(8, !InputPin::read(7)):%ul ns", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (InputPin::read(8))
	OutputPin::write(8, 0);
      else
	OutputPin::write(8, 1);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read-write pin:OutputPin::read(7)/write(8,0/1):%ul ns\n", ns);

  // Measure the time to perform 1,000 byte/8,000 bit data transfer
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    dataPin.write(data, clockPin);
    __asm__ __volatile__("nop");
  }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write serial(8):pin.write(data,clk):%ul ns", ns);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      clockPin.write(1);
      clockPin.write(0);
      __asm__ __volatile__("nop");
    }
  }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write serial(8)::pin.write();clock.write(1/0):%ul ns", ns);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      clockPin.toggle();
      clockPin.toggle();
      __asm__ __volatile__("nop");
    }
  }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write serial(8):pin.write/toggle():%ul ns", ns);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(9, data & bit);
      OutputPin::write(10, 1);
      OutputPin::write(10, 0);
      __asm__ __volatile__("nop");
    }
  }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write serial(8):OutputPin::write():%ul ns", ns);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(9, data & bit);
      OutputPin::toggle(10);
      OutputPin::toggle(10);
      __asm__ __volatile__("nop");
    }
  }
  stop = RTC::micros();
  ns = (stop - start) / 8000L;
  INFO("write serial(8):OutputPin::write/toggle():%ul ns", ns);
  
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
    __asm__ __volatile__("nop");
  }

  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("write serial(8):pin.write/toggle() unrolled:%ul ns\n", ns);

  // Measure the time to perform 1,000 analog pin samples
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    analogPin.sample();
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read analog pin:analogPin.sample():%ul us", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    analogPin.sample_request();
    analogPin.sample_await();
  }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read analog pin:analogPin.sample_request/await():%ul us", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint16_t var; 
    analogPin >> var;
  }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read analog pin:analogPin >> var:%ul us", ns);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    AnalogPin::sample(0);
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("read analog pin:AnalogPin::sample():%ul us\n", ns);

  // Measure the time to perform 1,000 analog pin samples
  // Vary prescale division factor from 128 down to 2.
  for (uint8_t factor = 7; factor > 0; factor--) {
    AnalogPin::prescale(factor);
    start = RTC::micros();
    for (uint16_t i = 0; i < 1000; i++)
      analogPin.sample();
    stop = RTC::micros();
    ns = (stop - start) / 1000L;
    INFO("read analog pin:prescale(%d):bits(%d):analogPin.sample():%ul us", 
	 1 << factor, factor + 3, ns);
  }
  trace.println();

  // Stop the benchmark
  ASSERT(true == false);
}
