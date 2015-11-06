/**
 * @file CosaBenchmarkPins.ino
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
#include "Cosa/RTT.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Pins used in the benchmark (Note will not compile for ATtinyX5)

InputPin inPin(Board::D7);
OutputPin outPin(Board::D8);
OutputPin dataPin(Board::D9);
OutputPin clockPin(Board::D10);
AnalogPin analogPin(Board::A0);

// Simple adaptation of the Arduino/Wiring API but with strong
// data typed pins
inline void pinMode(Board::DigitalPin pin, uint8_t mode)
  __attribute__((always_inline));

inline void pinMode(Board::DigitalPin pin, uint8_t mode)
{
  IOPin::mode(pin, (IOPin::Mode) mode);
}

inline int digitalRead(Board::DigitalPin pin)
  __attribute__((always_inline));

inline int digitalRead(Board::DigitalPin pin)
{
  return (InputPin::read(pin));
}

inline void digitalWrite(Board::DigitalPin pin, uint8_t value)
  __attribute__((always_inline));

inline void digitalWrite(Board::DigitalPin pin, uint8_t value)
{
  OutputPin::write(pin, value);
}

inline void digitalToggle(Board::DigitalPin pin)
  __attribute__((always_inline));

inline void digitalToggle(Board::DigitalPin pin)
{
  OutputPin::toggle(pin);
}

inline int analogRead(Board::AnalogPin pin)
  __attribute__((always_inline));

inline int analogRead(Board::AnalogPin pin)
{
  return (AnalogPin::sample(pin));
}

void setup()
{
  // Start the timers
  Watchdog::begin();
  RTT::begin();

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

  // Powerup ADC
  AnalogPin::powerup();
}

#define MEASURE_SUITE(msg)						\
  trace << endl;							\
  INFO(msg, 0);

#define MEASURE_NS(msg)							\
  trace.flush();							\
  start = RTT::micros();						\
  for (uint8_t n = 1;							\
       n != 0;								\
       n--,								\
       stop = RTT::micros(),						\
       ns = (stop - start) / 1000L,					\
       trace << __LINE__ << ':' << __PRETTY_FUNCTION__,			\
       trace << PSTR(":measure:") << PSTR(msg),				\
       trace << PSTR(":") << ns - baseline,				\
       trace << PSTR(" ns") << endl)					\
    for (uint16_t i = 0; i < 1000; i++)					\
      for (uint16_t j = 0; j < 1000; j++)

#define MEASURE_US(msg)							\
  trace.flush();							\
  start = RTT::micros();						\
  for (uint8_t n = 1;							\
       n != 0;								\
       n--,								\
       stop = RTT::micros(),						\
       ns = stop - start,						\
       trace << __LINE__ << ':' << __PRETTY_FUNCTION__,			\
       trace << PSTR(":measure:") << PSTR(msg),				\
       trace << PSTR(":") << ns / 1000L,				\
       trace << PSTR(" us") << endl)					\
    for (uint16_t i = 0; i < 1000; i++)

void loop()
{
  uint32_t baseline = 0, start, stop;
  uint32_t ns;
  uint8_t cnt = 0;

  MEASURE_SUITE("Measure the time to perform an empty loop block");

  MEASURE_NS("nop") {
    __asm__ __volatile__("nop");
  }
  baseline = ns;

  MEASURE_SUITE("Measure the time to perform an input pin read");

  MEASURE_NS("inPin.is_set()") {
    cnt += inPin.is_set();
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("inPin >> var") {
    uint8_t var;
    inPin >> var;
    cnt += var;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("var = inPin") {
    bool var;
    var = inPin;
    cnt += var;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("InputPin::read(D7)") {
    uint8_t var = InputPin::read(Board::D7);
    cnt += var;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("digitalRead(D7)") {
    uint8_t var = digitalRead(Board::D7);
    cnt += var;
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform an output pin toggle");

  MEASURE_NS("outPin.write(x)/x=!x") {
    static bool var = false;
    outPin.write(var);
    var = !var;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin.write(1/0)") {
    outPin.write(0);
    outPin.write(1);
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin._write(1/0)") {
    synchronized {
      outPin._write(0);
      outPin._write(1);
    }
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin.set/clear()") {
    outPin.set();
    outPin.clear();
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("synchronized outPin._set/_clear()") {
    synchronized {
      outPin._set();
      outPin._clear();
    }
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin << 1/0") {
    outPin << 1;
    outPin << 0;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin = 1/0") {
    outPin = 1;
    outPin = 0;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("OutputPin::write(D8, 1/0)") {
    OutputPin::write(Board::D8, 1);
    OutputPin::write(Board::D8, 0);
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin.toggle()") {
    outPin.toggle();
    outPin.toggle();
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("synchronized outPin._toggle()") {
    synchronized {
      outPin._toggle();
      outPin._toggle();
    }
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("OutputPin::toggle(D8)") {
    OutputPin::toggle(Board::D8);
    OutputPin::toggle(Board::D8);
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("digitalWrite(D8, 1/0)") {
    digitalWrite(Board::D8, 1);
    digitalWrite(Board::D8, 0);
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("digitalToggle(D8)") {
    digitalToggle(Board::D8);
    digitalToggle(Board::D8);
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform input pin read/output pin write");

  MEASURE_NS("outPin.write(!inPin.read())") {
    outPin.write(!inPin.read());
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("inPin.is_set();outPin.clear/set()") {
    if (inPin.is_set())
      outPin.clear();
    else
      outPin.set();
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("inPin >> var; outPin << !var") {
    uint8_t var;
    inPin >> var;
    outPin << !var;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin = !inPin") {
    outPin = !inPin;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("outPin.set(inPin.is_clear())") {
    outPin.set(inPin.is_clear());
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("OutputPin::write(D8, !InputPin::read(D7))") {
    OutputPin::write(Board::D8, !InputPin::read(Board::D7));
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("OutputPin::read(D7)/write(D8,0/1)") {
    if (InputPin::read(Board::D8))
      OutputPin::write(Board::D8, 0);
    else
      OutputPin::write(Board::D8, 1);
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("digitalWrite(D8, !digitalRead(D7))") {
    digitalWrite(Board::D8, !digitalRead(Board::D7));
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("digitalRead(D7)/digitalWrite(D8,0/1)") {
    if (digitalRead(Board::D8))
      digitalWrite(Board::D8, 0);
    else
      digitalWrite(Board::D8, 1);
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform 8-bit serial data transfer");

  MEASURE_US("pin.write(data,clk)") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      clockPin.write(1);
      clockPin.write(0);
    }
  }

  MEASURE_US("pin.write();clock.write(1/0)") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      clockPin.write(1);
      clockPin.write(0);
    }
  }

  MEASURE_US("pin._write();clock._write(1/0)") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      synchronized {
	dataPin._write(data & bit);
	clockPin._write(1);
	clockPin._write(0);
      }
    }
  }

  MEASURE_US("pin.write/toggle()") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      clockPin.toggle();
      clockPin.toggle();
    }
  }

  MEASURE_US("pin.write/_toggle()") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin.write(data & bit);
      synchronized {
	clockPin._toggle();
	clockPin._toggle();
      }
    }
  }

  MEASURE_US("pin._write/_toggle()") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      synchronized {
	dataPin._write(data & bit);
	clockPin._toggle();
	clockPin._toggle();
      }
    }
  }

  MEASURE_US("OutputPin::write()") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(Board::D9, data & bit);
      OutputPin::write(Board::D10, 1);
      OutputPin::write(Board::D10, 0);
    }
  }

  MEASURE_US("OutputPin::write/toggle()") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      OutputPin::write(Board::D9, data & bit);
      OutputPin::toggle(Board::D10);
      OutputPin::toggle(Board::D10);
    }
  }

  MEASURE_US("pin.write/toggle() unrolled") {
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

  MEASURE_US("digitalWrite(D9/D10)") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      digitalWrite(Board::D9, data & bit);
      digitalWrite(Board::D10, 1);
      digitalWrite(Board::D10, 0);
    }
  }

  MEASURE_US("digitalWrite(D9)/digitalToggle(D10)") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      digitalWrite(Board::D9, data & bit);
      digitalToggle(Board::D10);
      digitalToggle(Board::D10);
    }
  }

  MEASURE_SUITE("Measure the time to read analog pin");

  MEASURE_US("analogPin.sample()") {
    analogPin.sample();
  }

  MEASURE_US("analogPin.sample_request/await()") {
    analogPin.sample_request();
    analogPin.sample_await();
  }

  MEASURE_US("analogPin >> var") {
    uint16_t var;
    analogPin >> var;
  }

  MEASURE_US("var = analogPin") {
    uint16_t var = analogPin;
    UNUSED(var);
  }

  MEASURE_US("AnalogPin::sample(A0)") {
    AnalogPin::sample(Board::A0);
  }

  MEASURE_US("AnalogPin::bandgap()") {
    AnalogPin::bandgap();
  }

  MEASURE_US("analogRead(A0)") {
    analogRead(Board::A0);
  }

  MEASURE_SUITE("Measure the time to read analog pin with varying prescale");
  for (uint8_t factor = 7; factor > 0; factor--) {
    AnalogPin::prescale(factor);
    start = RTT::micros();
    for (uint16_t i = 0; i < 1000; i++)
      analogPin.sample();
    stop = RTT::micros();
    ns = (stop - start);
    INFO("prescale(%d):bits(%d):analogPin.sample():%ul us",
	 1 << factor, factor + 3, ns / 1000L);
  }
  trace.println();

  // Stop the benchmark
  DEBUG("cnt = %d", cnt);
  ASSERT(true == false);
}

