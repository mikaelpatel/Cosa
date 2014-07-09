/**
 * @file CosaBenchmarkPins.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * Cosa Pins Benchmark; number of micro-seconds for pin operations.
 *
 * Measurement of the pins operations; digital/analog input pin read,
 * digital output pin write and toggle, and serial output with clock.  
 * 
 * Reading a digital pin in Cosa is 6-7X faster than Arduino. Writing
 * is 2-10X faster. Serial output with data and clock pin is 4X
 * faster. Reading an analog pin in Cosa is equal to Arduino though 
 * Cosa allows the ADC to be performed asynchroniously. 
 * 
 * The speedup can be explained by the caching of port register
 * pointer and pin mask in Cosa compared to Arduino. Also most access
 * functions in Cosa are inlined. Though object-oriented and in/output
 * operator syntax Cosa is between 2-10X faster allowing high speed
 * protocols. 
 *
 * The digital pin object holds reference to special function register
 * (port), pin mask and pin number (total of 4 bytes). The analog pin
 * object holds the ADC channel number, latest sample, reference
 * voltage, and allows an interrupt and event handler (total 9 bytes).
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output,
 * and pins D7-D9 and A0.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOPin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Pins used in the benchmark (Note will not compile for ATtinyX5)

InputPin inPin(Board::D7);
OutputPin outPin(Board::D8);
OutputPin dataPin(Board::D9);
OutputPin clockPin(Board::D10);
AnalogPin analogPin(Board::A0);

// Simple adaptation of the Arduino/Wiring API but with strong
// data typed pins
inline void digitalWrite(Board::DigitalPin pin, uint8_t value)
{
  OutputPin::write(pin, value);
}

inline int digitalRead(Board::DigitalPin pin)
{
  return (InputPin::read(pin));
}

inline int analogRead(Board::AnalogPin pin)
{
  return (AnalogPin::sample(pin));
}

inline void pinMode(Board::DigitalPin pin, uint8_t mode)
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
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  uint32_t baseline, start, stop;
  uint32_t ns;
  uint8_t cnt = 0;

  INFO("Measure the time to perform an empty loop block", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  baseline = ns = (stop - start) / 1000L;
  INFO("nop:%ul ns\n", ns);

  INFO("Measure the time to perform an input pin read", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      cnt += inPin.is_set();
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("inPin.is_set():%ul ns", ns - baseline);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      uint8_t var;
      inPin >> var;
      cnt += var;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("inPin >> var:%ul ns", ns - baseline);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      cnt += InputPin::read(Board::D7);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("InputPin::read(D7):%ul ns", ns - baseline);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      cnt += digitalRead(Board::D7);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("digitalRead(D7):%ul ns\n", ns - baseline);
  
  INFO("Measure the time to perform an output pin write", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.write(1);
      outPin.write(0);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin.write():%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      synchronized {
	outPin._write(1);
	outPin._write(0);
      }
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin._write():%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.set();
      outPin.clear();
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin.set/clear():%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      synchronized {
	outPin._set();
	outPin._clear();
      }
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin._set/_clear():%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin << 1;
      outPin << 0;
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin << val:%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::write(Board::D8, 1);
      OutputPin::write(Board::D8, 0);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("OutputPin::write(D8, val):%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      digitalWrite(Board::D8, 1);
      digitalWrite(Board::D8, 0);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("digitalWrite(D8, val):%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.toggle();
      outPin.toggle();
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin.toggle():%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      synchronized {
	outPin._toggle();
	outPin._toggle();
      }
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin._toggle():%ul ns", (ns - baseline) / 2);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::toggle(Board::D8);
      OutputPin::toggle(Board::D8);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("OutputPin::toggle(D8):%ul ns\n", (ns - baseline) / 2);
  
  INFO("Measure the time to perform input pin read/output pin write", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.write(!inPin.read());
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin.write(!inPin.read()):%ul ns", ns - baseline);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (inPin.is_set()) outPin.clear(); else outPin.set();
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("inPin.is_set();outPin.clear/set():%ul ns", ns - baseline);

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
  INFO("inPin >> var; outPin << !var:%ul ns", ns - baseline);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      outPin.set(inPin.is_clear());
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("outPin.set(inPin.is_clear()):%ul ns", ns - baseline);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      OutputPin::write(Board::D8, !InputPin::read(Board::D7));
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("OutputPin::write(D8, !InputPin::read(D7)):%ul ns", ns - baseline);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++) {
      if (InputPin::read(Board::D8))
	OutputPin::write(Board::D8, 0);
      else
	OutputPin::write(Board::D8, 1);
      __asm__ __volatile__("nop");
    }
  stop = RTC::micros();
  ns = (stop - start) / 1000L;
  INFO("OutputPin::read(D7)/write(D8,0/1):%ul ns\n", ns);

  INFO("Measure the time to perform 8-bit serial data transfer", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    dataPin.write(data, clockPin);
  }
  stop = RTC::micros();
  ns = (stop - start);
  INFO("pin.write(data,clk):%ul us", ns / 1000L);
  
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
  ns = (stop - start);
  INFO("pin.write();clock.write(1/0):%ul us", ns / 1000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      synchronized {
	dataPin._write(data & bit);
	clockPin._write(1);
	clockPin._write(0);
      }
    }
  }
  stop = RTC::micros();
  ns = (stop - start);
  INFO("pin._write();clock._write(1/0):%ul us", ns / 1000L);
  
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
  ns = (stop - start);
  INFO("pin.write/toggle():%ul us", ns  / 1000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      synchronized {
	dataPin._write(data & bit);
	clockPin._toggle();
	clockPin._toggle();
      }
    }
  }
  stop = RTC::micros();
  ns = (stop - start);
  INFO("pin._write/_toggle():%ul us", ns  / 1000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(Board::D9, data & bit);
      OutputPin::write(Board::D10, 1);
      OutputPin::write(Board::D10, 0);
    }
  }
  stop = RTC::micros();
  ns = (stop - start);
  INFO("OutputPin::write():%ul us", ns / 1000L);
  
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(Board::D9, data & bit);
      OutputPin::toggle(Board::D10);
      OutputPin::toggle(Board::D10);
    }
  }
  stop = RTC::micros();
  ns = (stop - start);
  INFO("OutputPin::write/toggle():%ul us", ns / 1000L);
  
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
  ns = (stop - start);
  INFO("pin.write/toggle() unrolled:%ul us\n", ns / 1000L);

  INFO("Measure the time to read analog pin", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    analogPin.sample();
  stop = RTC::micros();
  ns = (stop - start);
  INFO("analogPin.sample():%ul us", ns / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    analogPin.sample_request();
    analogPin.sample_await();
  }
  stop = RTC::micros();
  ns = (stop - start);
  INFO("analogPin.sample_request/await():%ul us", ns / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++) {
    uint16_t var; 
    analogPin >> var;
  }
  stop = RTC::micros();
  ns = (stop - start);
  INFO("analogPin >> var:%ul us", ns / 1000L);

  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    AnalogPin::sample(Board::A0);
  stop = RTC::micros();
  ns = (stop - start);
  INFO("AnalogPin::sample(A0):%ul us\n", ns / 1000L);

  INFO("Measure the time to read analog pin with varying prescale", 0);
  for (uint8_t factor = 7; factor > 0; factor--) {
    AnalogPin::prescale(factor);
    start = RTC::micros();
    for (uint16_t i = 0; i < 1000; i++)
      analogPin.sample();
    stop = RTC::micros();
    ns = (stop - start);
    INFO("prescale(%d):bits(%d):analogPin.sample():%ul us", 
	 1 << factor, factor + 3, ns / 1000L);
  }
  trace.println();

  // Stop the benchmark
  DEBUG("cnt = %d", cnt);
  ASSERT(true == false);
}

