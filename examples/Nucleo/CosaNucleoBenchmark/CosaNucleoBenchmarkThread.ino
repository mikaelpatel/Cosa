/**
 * @file CosaNucleoBenchmark.ino
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
 * Cosa Nucleo Thread benchmarks
 * 1) Run 1,000,000 context switches (12-13 us)
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Nucleo/Thread.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class Benchmark : public Nucleo::Thread {
public:
  virtual void run();
};

void 
Benchmark::run() 
{ 
  // This will force 1,000,000 context switches as the Nucleo main thread
  // will context switch back for each loop; 2*500*1000 in total
  // The loop handling and RTC access are included but very small values.
  while (1) {
    uint32_t start = RTC::micros();
    for (uint16_t i = 0; i < 500; i++)
      for (uint16_t j = 0; j < 1000; j++)
	yield(); 
    uint32_t stop = RTC::micros();
    trace << PSTR("yield: ") << (stop - start) / 1000000 << PSTR(" us") << endl;
  }
}

Benchmark bench;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoBenchmark: started"));
  Watchdog::begin();
  RTC::begin();
  Nucleo::Thread::begin(&bench, 128);
}

void loop()
{
  Nucleo::Thread::begin();
  ASSERT(true == false);
}
