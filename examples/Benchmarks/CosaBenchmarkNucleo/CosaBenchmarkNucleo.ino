/**
 * @file CosaBenchmarkNucleo.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * Cosa Nucleo benchmarks; 
 * 1) Thread context switches, measured with yield
 * 2) Thread context switches, measured with resume(this)
 * 3) Semaphore signal-wait, measured between two threads
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output,
 * internal timer for RTC and watchdog.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Nucleo/Thread.hh"
#include "Cosa/Nucleo/Semaphore.hh"
#include "Cosa/IOStream/Driver/UART.hh"

Nucleo::Semaphore sem(0);

class Producer : public Nucleo::Thread {
public:
  virtual void run();
};

void 
Producer::run() 
{ 
  trace << PSTR("Thread::Producer: started") << endl;  

  while (1) {
    uint32_t start, us;

    // Measure 500,000 yield will give 1,000,000 context switches
    // as the main (background) thread will be run
    INFO("Benchmark 1: measure yield", 0);
    start = RTC::micros();
    for (uint16_t i = 0; i < 500; i++)
      for (uint16_t j = 0; j < 1000; j++)
	yield(); 
    us = (RTC::micros() - start)  / 1000000L;
    INFO("%l us", us);
    
    // Measure 1,000,000 resume to the current thread
    INFO("Benchmark 2: measure resume", 0);
    start = RTC::micros();
    for (uint16_t i = 0; i < 1000; i++)
      for (uint16_t j = 0; j < 1000; j++)
	resume(this);
    us = (RTC::micros() - start) / 1000000L;
    INFO("%l us", us);
  
    // Measure 100,000 signal-wait pairs
    INFO("Benchmark 3: measure signal-wait", 0);
    start = RTC::micros();
    for (uint8_t i = 0; i < 100; i++)
      for (uint16_t j = 0; j < 1000; j++)
	sem.signal();
    us = (RTC::micros() - start) / 100000L;
    INFO("%l us", us);

    trace << endl;
  }
}

class Consumer : public Nucleo::Thread {
public:
  virtual void run();
};

void
Consumer::run()
{
  trace << PSTR("Thread::Consumer: started") << endl;  
  while (1) sem.wait();
}

Producer producer;
Consumer consumer;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkNucleo: started"));
  Watchdog::begin();
  RTC::begin();
  Nucleo::Thread::begin(&consumer, 64);
  Nucleo::Thread::begin(&producer, 64);
}

void loop()
{
  Nucleo::Thread::begin();
  ASSERT(true == false);
}

/**
@section Output
CosaBenchmarkNucleo: started
Thread::Consumer: started
Thread::Producer: started
60:virtual void Producer::run():info:Benchmark 1: measure yield
66:virtual void Producer::run():info:11 us
69:virtual void Producer::run():info:Benchmark 2: measure resume
75:virtual void Producer::run():info:11 us
78:virtual void Producer::run():info:Benchmark 3: measure signal-wait
84:virtual void Producer::run():info:36 us
*/
