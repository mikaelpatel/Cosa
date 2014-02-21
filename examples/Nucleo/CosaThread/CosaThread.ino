/**
 * @file CosaThread.ino
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

#include "Cosa/Nucleo/Thread.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class Echo : public Nucleo::Thread {
private:
  const char* m_name;
public:
  Echo() : Thread(), m_name(NULL) {};
  void begin(const char* name, size_t size);
  virtual void run();
  void fn0(uint8_t& nr) { yield(); nr += 1; }
  void fn1(uint8_t& nr) { fn0(nr); }
  void fn2(uint8_t& nr) { fn1(nr); }
};

void
Echo::begin(const char* name, size_t size)
{ 
  m_name = name; 
  Thread::begin(this, size);
}

void
Echo::run()
{
  uint8_t nr = 0;
  while (1) {
    trace << nr << ':' << m_name << endl;
    delay(1000);
    fn2(nr);
  }
}

Echo foo;
Echo fie;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaThread: started"));
  Watchdog::begin();

  TRACE(sizeof(Nucleo::Thread));
  TRACE(sizeof(Echo));

  foo.begin(PSTR("foo"), 128);
  fie.begin(PSTR("fie"), 128);
}

void loop()
{
  Nucleo::Thread::begin();
  ASSERT(true == false);
}
