/**
 * @file CosaNucleoBenchmark.ino
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

#include <Nucleo.h>

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

Nucleo::Semaphore sem(0);
uint16_t count = 0;

class Producer : public Nucleo::Thread {
public:
  virtual void run()
  {
    trace << PSTR("Thread::Producer: started") << endl;
    while (1) {
      // Measure 1,000 resume to the current thread
      MEASURE("thread resume: ", 1000) {
	resume(this);
      }
      // Measure 1,000 yield will give 2,000 context switches
      // as the main (background) thread will be run
      MEASURE("thread yield: ", 1000) {
	yield();
      }
      // Measure 1,000 signal-wait pairs
      MEASURE("semaphore signal-wait: ", 1000) {
	sem.signal();
      }
      TRACE(count);
      // Stop the benchmark run
      ASSERT(true == false);
    }
  }
};

class Consumer : public Nucleo::Thread {
public:
  virtual void run()
  {
    trace << PSTR("Thread::Consumer: started") << endl;
    while (1) {
      sem.wait();
      count++;
    }
  }
};

// The threads
Producer producer;
Consumer consumer;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkNucleo: started"));
  trace.flush();

  // Start timers
  Watchdog::begin();
  RTT::begin();

  // Start threads
  Nucleo::Thread::begin(&consumer, 64);
  Nucleo::Thread::begin(&producer, 64);
  Nucleo::Thread::begin();
}

void loop()
{
  // Run the kernel
  Nucleo::Thread::service();
}
