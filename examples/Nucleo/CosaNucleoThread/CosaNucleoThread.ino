/**
 * @file CosaNucleoThread.ino
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
 * Demonstration of Cosa Nucleo Threads.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Nucleo/Thread.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class Echo : public Nucleo::Thread {
private:
  const char* m_name;
  uint16_t m_ms;
public:
  // Construct echo thread 
  Echo(uint16_t ms) : Thread(), m_name(NULL), m_ms(ms) {};

  // Allocate stack and initiate name string
  void begin_P(const char* name, size_t size);

  // Echo thread function
  virtual void run();

  // A function tree to show that the stack is maintained
  void fn0(uint8_t& nr) { fn1(nr); }
  void fn1(uint8_t& nr) { fn2(nr); }
  void fn2(uint8_t& nr) { yield(); nr += 1; }
};

void
Echo::begin_P(const char* name, size_t size)
{ 
  m_name = name; 
  Thread::begin(this, size);
}

void
Echo::run()
{
  uint8_t nr = 0;
  while (1) {
    trace << Watchdog::millis() << PSTR(":Echo:") << m_name << ':' << nr << endl;
    delay(m_ms);
    fn0(nr);
  }
}

// Two echo threads
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

  // Initiate the two threads (stack size 128)
  foo.begin_P(PSTR("foo"), 128);
  fie.begin_P(PSTR("fie"), 128);
}

void loop()
{
  // Run the threads; start the main thread
  Nucleo::Thread::begin();

  // Sanity check; should never come here
  ASSERT(true == false);
}
