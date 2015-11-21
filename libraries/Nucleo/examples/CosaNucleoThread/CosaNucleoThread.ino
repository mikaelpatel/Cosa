/**
 * @file CosaNucleoThread.ino
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
 * Demonstration of Cosa Nucleo Threads.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

Nucleo::Semaphore io(1);

class Echo : public Nucleo::Thread {
public:
  // Construct echo thread
  Echo(uint16_t ms) : Thread(), m_name(NULL), m_ms(ms) {};

  // Allocate stack and initiate name string
  void begin_P(str_P name, size_t size);

  // Echo thread function
  virtual void run();

  // A function tree to show that the stack is maintained
  void fn0(uint16_t& nr)  __attribute__((noinline));
  void fn1(uint16_t& nr)  __attribute__((noinline));
  void fn2(uint16_t& nr)  __attribute__((noinline));

private:
  str_P m_name;
  uint16_t m_ms;
};

void
Echo::fn0(uint16_t& nr)
{
  mutex(io) INFO("nr=%d", nr);
  fn1(nr);
}

void
Echo::fn1(uint16_t& nr)
{
  mutex(io) INFO("nr=%d", nr);
  fn2(nr);
}

void
Echo::fn2(uint16_t& nr)
{
  yield();
  nr += 1;
  mutex(io) INFO("nr=%d", nr);
}

void
Echo::begin_P(str_P name, size_t size)
{
  m_name = name;
  Thread::begin(this, size);
}

void
Echo::run()
{
  uint16_t nr = 0;
  while (1) {
    mutex(io) {
      trace << Watchdog::millis() << PSTR(":Echo:")
	    << m_name << ':' << nr
	    << endl;
    }
    delay(m_ms);
    fn0(nr);
  }
}

// Two echo threads with different delays
Echo foo(1500);
Echo fie(1000);

void setup()
{
  // Setup trace output stream and start watchdog timer
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoThread: started"));
  Watchdog::begin();

  // Some information about memory foot print
  TRACE(sizeof(jmp_buf));
  TRACE(sizeof(Nucleo::Thread));
  TRACE(sizeof(Echo));

  // Initiate the two threads (stack size 256)
  foo.begin_P(PSTR("foo"), 256);
  fie.begin_P(PSTR("fie"), 256);

  // Start the main thread
  Nucleo::Thread::begin();
}

void loop()
{
 // Service the nucleos
  Nucleo::Thread::service();
}
