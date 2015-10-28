/**
 * @file CosaNucleoPingPong.ino
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
 * Demonstration of Cosa Nucleo Threads and Semaphores; 1) control
 * signalling between threads, 2) mutual exclusive access to resource
 * (trace output stream).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

uint32_t start = 0UL;

Nucleo::Semaphore sem(0);
Nucleo::Semaphore io(1);

class Ping : public Nucleo::Thread {
public:
  virtual void run()
  {
    uint16_t cycle = 0;
    mutex(io) {
      trace << PSTR("Ping: started") << endl;
    }
    while (1) {
      start = RTT::micros();
      sem.signal();
      mutex(io) {
	trace << Watchdog::millis()
	      << PSTR(":Ping")
	      << PSTR(",cycle=") << cycle++
	      << endl;
      }
      delay(1000);
    }
  }
};

class Pong : public Nucleo::Thread {
public:
  virtual void run()
  {
    uint16_t cycle = 0;
    mutex(io) {
      trace << PSTR("Pong: started") << endl;
    }
    while (1) {
      sem.wait();
      uint32_t us = RTT::micros() - start;
      mutex(io) {
	trace << Watchdog::millis()
	      << PSTR(":Pong")
	      << PSTR(",cycle=") << cycle++
	      << PSTR(",us=") << us
	      << endl;
      }
    }
  }
};

// The threads
Ping ping;
Pong pong;

void setup()
{
  // Setup trace output stream and start watchdog timer
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoPingPong: started"));
  trace.flush();
  Watchdog::begin();
  RTT::begin();

  // Some information about memory foot print
  TRACE(sizeof(jmp_buf));
  TRACE(sizeof(Nucleo::Thread));
  TRACE(sizeof(Ping));
  TRACE(sizeof(Pong));
  TRACE(sizeof(Nucleo::Semaphore));

  // Initiate the two threads (stack size 128)
  Nucleo::Thread::begin(&pong, 128);
  Nucleo::Thread::begin(&ping, 128);

  // Start the main thread
  Nucleo::Thread::begin();
}

void loop()
{
  // Service the nucleos
  Nucleo::Thread::service();
}
