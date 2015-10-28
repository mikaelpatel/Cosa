/**
 * @file CosaNucleoActor.ino
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
 * Demonstration of Cosa Nucleo Actors and message passing.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

// Send message to given consumer actor and port
class Producer : public Nucleo::Actor {
public:
  Producer(Actor* consumer, uint8_t port) :
    Actor(),
    m_consumer(consumer),
    m_port(port)
  {}
  virtual void run();
private:
  Actor* m_consumer;
  uint8_t m_port;
};

void
Producer::run()
{
  uint16_t count = 0;
  while (1) {
    trace << Watchdog::millis()
	  << PSTR(":Producer:count=") << count
	  << endl;
    m_consumer->send(m_port, &count, sizeof(count));
    count += 1;
  }
}

// Receive messages from producer actors
class Consumer : public Nucleo::Actor {
public:
  virtual void run();
};

void
Consumer::run()
{
  while (1) {
    Actor* sender = NULL;
    uint8_t port = 0;
    uint16_t count = 0;
    recv(sender, port, &count, sizeof(count));
    trace << Watchdog::millis()
	  << PSTR(":Consumer:sender=") << sender
	  << PSTR(",port=") << port
	  << PSTR(",count=") << count
	  << endl;
    delay(500);
  }
}

// Single consumter and two actors
Consumer consumer;
Producer producer1(&consumer, 1);
Producer producer2(&consumer, 2);

void setup()
{
  // Start serial as trace iostream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoActor: started"));

  // Start watchdog timer as clock
  Watchdog::begin();

  // Start the actors
  Nucleo::Thread::begin(&producer1, 128);
  Nucleo::Thread::begin(&producer2, 128);
  Nucleo::Thread::begin(&consumer, 128);

  // Start the main thread
  Nucleo::Thread::begin();
}

void loop()
{
  // Service the nucleos
  Nucleo::Thread::service();
}
