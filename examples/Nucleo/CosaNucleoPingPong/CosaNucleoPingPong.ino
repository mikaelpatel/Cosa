/**
 * @file CosaNucleoPingPong.ino
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
 * Demonstration of Cosa Nucleo Threads and Semaphores; 
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

class Ping : public Nucleo::Thread {
public:
  virtual void run();
};
 
class Pong : public Nucleo::Thread {
public:
  virtual void run();
};

void
Ping::run()
{
  trace << PSTR("Ping: started") << endl;
  while (1) {
    sem.signal();
    trace << Watchdog::millis() << PSTR(":Ping") << endl;
    delay(1000);
  }
}

void
Pong::run()
{
  trace << PSTR("Pong: started") << endl;
  while (1) {
    sem.wait();
    trace << Watchdog::millis() << PSTR(":Pong") << endl;
  }
}

Ping ping;
Pong pong;

void setup()
{
  // Setup trace output stream and start watchdog timer
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoPingPong: started"));
  Watchdog::begin();
  RTC::begin();

  // Some information about memory foot print
  TRACE(sizeof(jmp_buf));
  TRACE(sizeof(Nucleo::Thread));
  TRACE(sizeof(Nucleo::Semaphore));

  // Initiate the two threads (stack size 128)
  Nucleo::Thread::begin(&ping, 128);
  Nucleo::Thread::begin(&pong, 128);
}

void loop()
{
  // Run the threads; start the main thread
  Nucleo::Thread::begin();

  // Sanity check; should never come here
  ASSERT(true == false);
}
