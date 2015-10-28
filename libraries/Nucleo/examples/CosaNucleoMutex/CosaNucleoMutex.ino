/**
 * @file CosaNucleoMutex.ino
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
 * Demonstration of Cosa Nucleo Threads, Semaphores and Mutex.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

Nucleo::Semaphore sem;

uint8_t c1 = 0;
uint8_t c2 = 0;

class Counter : public Nucleo::Thread {
private:
  uint8_t m_id;
  uint16_t m_ms;
public:
  Counter(uint8_t id, uint16_t ms) : m_id(id), m_ms(ms) {}
  virtual void run()
  {
    while (1) {
      uint8_t x1 = c1;

      // Mutual exclusive update of c2
      mutex(sem) {
	uint8_t x2 = c2;
	delay(m_ms);
	c2 = x2 + 1;
      }

      // Update c1 with value that actually might have changed
      c1 = x1 + 1;
      delay(m_ms);

      // Mutual exclusive trace the values
      {
	Nucleo::Mutex m(sem);
	trace << PSTR("id = ") << m_id
	      << PSTR(", c1 = ") << c1
	      << PSTR(", c2 = ") << c2
	      << endl;
      }
    }
  }
};

// Two counters with different delays to achieve interleaving
Counter count1(1, 1000);
Counter count2(2, 200);

void setup()
{
  // Setup trace output stream and start watchdog timer
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoMutex: started"));
  trace.flush();
  Watchdog::begin();

  // Some information about memory foot print
  TRACE(sizeof(jmp_buf));
  TRACE(sizeof(Nucleo::Thread));
  TRACE(sizeof(Counter));
  TRACE(sizeof(Nucleo::Semaphore));
  TRACE(sizeof(Nucleo::Mutex));

  // Initiate the two threads (stack size 128)
  Nucleo::Thread::begin(&count1, 128);
  Nucleo::Thread::begin(&count2, 128);

  // Start the main thread
  Nucleo::Thread::begin();
}

void loop()
{
  // Run the kernel
  Nucleo::Thread::service();
}

