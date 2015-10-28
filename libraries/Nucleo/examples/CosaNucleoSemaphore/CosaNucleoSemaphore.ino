/**
 * @file CosaNucleoSemaphore.ino
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
 * Demonstration of Cosa Nucleo Threads and Semaphores.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

Nucleo::Semaphore sem(0);

class Thread1 : public Nucleo::Thread {
public:
  virtual void run()
  {
    uint8_t nr = 0;
    while (1) {
      trace << Watchdog::millis() << PSTR(":Thread1:") << nr << endl;
      if (nr == 10) sem.signal();
      if (nr == 15) sem.signal();
      if (nr == 20) sem.wait();
      if (nr == 40) sem.wait();
      nr += 1;
      delay(1000);
    }
  }
};

class Thread2 : public Nucleo::Thread {
public:
  virtual void run()
  {
    uint8_t nr = 0;
    sem.wait(2);
    while (1) {
      trace << Watchdog::millis() << PSTR(":Thread2:") << nr << endl;
      if (nr == 20) sem.signal();
      if (nr == 40) sem.wait();
      nr += 1;
      delay(1000);
    }
  }
};

// The threads
Thread1 foo;
Thread2 fie;

void setup()
{
  // Setup trace output stream and start watchdog timer
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoSemaphore: started"));
  Watchdog::begin();

  // Some information about memory foot print
  TRACE(sizeof(jmp_buf));
  TRACE(sizeof(Nucleo::Thread));
  TRACE(sizeof(Nucleo::Semaphore));

  // Initiate the two threads (stack size 128)
  Nucleo::Thread::begin(&foo, 128);
  Nucleo::Thread::begin(&fie, 128);

  // Start the main thread
  Nucleo::Thread::begin();
}

void loop()
{
  // Service the nucleos
  Nucleo::Thread::service();
}
