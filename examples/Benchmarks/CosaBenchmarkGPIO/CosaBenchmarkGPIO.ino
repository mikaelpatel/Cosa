/**
 * @file CosaBenchmarkGPIO.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Cosa GPIO Benchmark; number of micro-seconds for pin operations.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/GPIO.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Pins used in the benchmark (Note will not compile for ATtinyX5)

GPIO inPin(Board::D7, GPIO::INPUT_MODE);
GPIO outPin(Board::D8, GPIO::OUTPUT_MODE);
GPIO dataPin(Board::D9, GPIO::OUTPUT_MODE);
GPIO clockPin(Board::D10, GPIO::OUTPUT_MODE);

void setup()
{
  // Start the timers
  Watchdog::begin();
  RTT::begin();

  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkGPIO: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(GPIO));

  // Print CPU clock and instructions per 1MHZ
  TRACE(F_CPU);
  TRACE(I_CPU);
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

  MEASURE_SUITE("Measure the time to perform an empty loop block");

  MEASURE_NS("nop") {
    __asm__ __volatile__("nop");
  }
  baseline = ns;

  MEASURE_SUITE("Measure the time to perform an input pin read");

  MEASURE_NS("var = inPin") {
    bool var = inPin;
    UNUSED(var);
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("var = InputPin::read(D7)") {
    bool var = GPIO::read(Board::D7);
    UNUSED(var);
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform an output pin write");

  MEASURE_NS("outPin = 1") {
    outPin = 1;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("GPIO::write(D8, 1)") {
    GPIO::write(Board::D8, 1);
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform an output pin toggle");

  MEASURE_NS("outPin = !outPin") {
    outPin = !outPin;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("GPIO::write(D8, !GPIO::read(D8))") {
    GPIO::write(Board::D8, !GPIO::read(Board::D8));
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("~outPin") {
    ~outPin;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("GPIO::toggle(D8)") {
    GPIO::toggle(Board::D8);
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform an output pin clock pulse");

  MEASURE_NS("outPin = 1;outPin = 0") {
    outPin = 1;
    outPin = 0;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("GPIO::write(D8, 1);GPIO::write(D8, 0)") {
    GPIO::write(Board::D8, 1);
    GPIO::write(Board::D8, 0);
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("~outPin;~outPin") {
    ~outPin;
    ~outPin;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("GPIO::toggle(D8);GPIO::toggle(D8)") {
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform input pin read/output pin write");

  MEASURE_NS("outPin = !inPin") {
    outPin = !inPin;
    __asm__ __volatile__("nop");
  }

  MEASURE_NS("GPIO::write(D8, !GPIO::read(D7))") {
    GPIO::write(Board::D8, !GPIO::read(Board::D7));
    __asm__ __volatile__("nop");
  }

  MEASURE_SUITE("Measure the time to perform 8-bit serial data transfer");

  MEASURE_US("datePin = bit;clockPin = 1;clockPin = 0") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin = (data & bit);
      clockPin = 1;
      clockPin = 0;
    }
  }

  MEASURE_US("GPIO::write(D9,bit);GPIO::write(D8,1);GPIO::write(D8,0)") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      GPIO::write(Board::D9, data & bit);
      GPIO::write(Board::D8, 1);
      GPIO::write(Board::D8, 0);
    }
  }

  MEASURE_US("datePin = bit;~clockPin;~clockPin") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      dataPin = (data & bit);
      ~clockPin;
      ~clockPin;
    }
  }

  MEASURE_US("GPIO::write(D9,bit);GPIO::toggle(D8);GPIO::toggle(D8)") {
    uint8_t data = 0x55;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      GPIO::write(Board::D9, data & bit);
      GPIO::toggle(Board::D8);
      GPIO::toggle(Board::D8);
    }
  }

  MEASURE_US("unrolled datePin = bit;~clockPin;~clockPin") {
    uint8_t data = 0x55;
    dataPin = (data & 0x80);
    ~clockPin;
    ~clockPin;
    dataPin = (data & 0x40);
    ~clockPin;
    ~clockPin;
    dataPin = (data & 0x20);
    ~clockPin;
    ~clockPin;
    dataPin = (data & 0x10);
    ~clockPin;
    ~clockPin;
    dataPin = (data & 0x08);
    ~clockPin;
    ~clockPin;
    dataPin = (data & 0x04);
    ~clockPin;
    ~clockPin;
    dataPin = (data & 0x02);
    ~clockPin;
    ~clockPin;
    dataPin = (data & 0x01);
    ~clockPin;
    ~clockPin;
  }

  MEASURE_US("unrolled GPIO::write(D9,bit);GPIO::toggle(D8);GPIO::toggle(D8)") {
    uint8_t data = 0x55;
    GPIO::write(Board::D9, data & 0x80);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    GPIO::write(Board::D9, data & 0x40);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    GPIO::write(Board::D9, data & 0x20);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    GPIO::write(Board::D9, data & 0x10);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    GPIO::write(Board::D9, data & 0x08);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    GPIO::write(Board::D9, data & 0x04);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    GPIO::write(Board::D9, data & 0x02);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
    GPIO::write(Board::D9, data & 0x01);
    GPIO::toggle(Board::D8);
    GPIO::toggle(Board::D8);
  }

  // Stop the benchmark
  ASSERT(true == false);
}
