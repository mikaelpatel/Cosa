/**
 * @file CosaNucleoBenchmarks.ino
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa Nucleo benchmarks; 
 * 1) Thread context switches, measured with yield (12 us)
 * 2) Thread context switches, measured with resume(this) (12 us)
 * 3) Semaphore signal-wait (56 us)
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
uint32_t nr = 0;

class Benchmark : public Nucleo::Thread {
public:
  virtual void run();
};

void 
Benchmark::run() 
{ 
  uint32_t start;
  uint32_t us;

  INFO("Benchmark 1: measure yield", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 500; i++)
    for (uint16_t j = 0; j < 1000; j++)
      yield(); 
  us = (RTC::micros() - start)  / 1000000L;
  INFO("%l us", us);

  INFO("Benchmark 2: measure resume", 0);
  start = RTC::micros();
  for (uint16_t i = 0; i < 1000; i++)
    for (uint16_t j = 0; j < 1000; j++)
      resume(this);
  us = (RTC::micros() - start) / 1000000L;
  INFO("%l us", us);
  
  INFO("Benchmark 3: measure signal-wait", 0);
  start = RTC::micros();
  while (nr != 100000L) sem.signal();
  us = (RTC::micros() - start) / nr;
  INFO("%l us", us);
  trace << endl;
  nr = 0;
}

class Waiting : public Nucleo::Thread {
public:
  virtual void run();
};

void
Waiting::run()
{
  while (1) {
    sem.wait();
    nr += 1;
  }
}

Benchmark bench;
Waiting waiting;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoBenchmarks: started"));
  Watchdog::begin();
  RTC::begin();
  Nucleo::Thread::begin(&waiting, 32);
  Nucleo::Thread::begin(&bench, 128);
}

void loop()
{
  Nucleo::Thread::begin();
  ASSERT(true == false);
}
