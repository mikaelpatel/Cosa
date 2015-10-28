/**
 * @file CosaNucleoActor.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Actor with variable list argument passing.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Nucleo.h>

#include "Cosa/Math.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

class Actor : public Nucleo::Thread {
public:
  Actor() : Nucleo::Thread(), m_queue(), m_client(NULL), m_nr(0) {}
  virtual void run();
  int send(Actor* actor, ...);
  int recv(uint32_t ms = 0L);
  virtual int on_recv(int op, va_list args);
protected:
  Head m_queue;
  Actor* m_client;
  uint16_t m_nr;
  va_list m_args;
  int m_res;
};

void
Actor::run()
{
  while (1) recv();
}

int
Actor::recv(uint32_t ms)
{
  uint32_t start = Watchdog::millis();
  while (m_queue.is_empty() &&
	 ((ms == 0L) || (Watchdog::since(start) < ms)))
    yield();
  if (m_queue.is_empty()) return (ETIME);
  m_client = (Actor*) m_queue.succ();
  va_list args = m_client->m_args;
  int op = va_arg(args, int);
  m_client->m_res = on_recv(op, args);
  dequeue(&m_queue, true);
  return (0);
}

int
Actor::on_recv(int op, va_list args)
{
  trace.flush();
  trace << this << PSTR("::on_recv(") << m_client << PSTR(", ") << op;
  trace.flush();
  for (int arg; (arg = va_arg(args, int)) != 0;) {
    trace << PSTR(", ") << arg;
    trace.flush();
  }
  trace << PSTR(", 0)") << endl;
  trace.flush();
  return (op);
}

int
Actor::send(Actor* actor, ...)
{
  trace.flush();
  trace << this  << ':' << m_nr++
	<< PSTR("::send(") << actor
	<< PSTR(") call")
	<< endl;
  trace.flush();
  va_start(m_args, actor);
  m_res = -EPERM;
  enqueue(&actor->m_queue);
  trace << this << ':' << m_nr++
	<< PSTR("::send(") << actor
	<< PSTR(") return (") << m_res << ')'
	<< endl;
  trace.flush();
  delay(rand(500, 5000));
  va_end(m_args);
  return (m_res);
}

enum op_t {
  FOO_OP = 8,
  FIE_OP = 9
};

struct foo_args_t {
  int16_t m_x;
  int16_t m_y;
  int16_t m_z;
  foo_args_t(int16_t x, int16_t y, int16_t z) :
    m_x(x), m_y(y), m_z(z)
  {}
};

struct foo_closure_t {
  op_t m_op;
  foo_args_t args;
  foo_closure_t(int16_t x, int16_t y, int16_t z) :
    m_op(FOO_OP), args(x, y, z)
  {}
};

struct fie_args_t {
  int8_t m_pos;
  float32_t m_gain;
  fie_args_t(int8_t pos, float32_t gain) :
    m_pos(pos), m_gain(gain)
  {}
};

struct fie_closure_t {
  op_t m_op;
  fie_args_t args;
  fie_closure_t(int8_t pos, float32_t gain) :
    m_op(FIE_OP), args(pos, gain)
  {}
};

class DemoServer : public Actor {
public:
  virtual int on_recv(int op, va_list args);
protected:
  int foo(va_list val);
  int fie(va_list val);
};

int
DemoServer::foo(va_list val)
{
  foo_args_t* args = (foo_args_t*) val;
  trace << this	<< PSTR("::foo(") << args->m_x
	<< PSTR(", ") << args->m_y
	<< PSTR(", ") << args->m_z
	<< ')' << endl;
  trace.flush();
  return (1);
}

int
DemoServer::fie(va_list val)
{
  fie_args_t* args = (fie_args_t*) val;
  trace << this << PSTR("::fie(") << args->m_pos
	<< PSTR(", ") << args->m_gain
	<< ')' << endl;
  trace.flush();
  return (2);
}

int
DemoServer::on_recv(int op, va_list args)
{
  int res = -1;
  switch (op) {
  case FOO_OP: res = foo(args); break;
  case FIE_OP: res = fie(args); break;
  default: res = Actor::on_recv(op, args);
  }
  delay(rand(100, 500));
  return (res);
}

class DemoClient : public Actor {
public:
  DemoClient(Actor* server) : Actor(), m_server(server) {}
  virtual void run();
  int foo(int16_t x, int16_t y, int16_t z);
  int fie(int8_t pos, float32_t gain);
private:
  Actor* m_server;
};

int
DemoClient::foo(int16_t x, int16_t y, int16_t z)
{
  return (send(m_server, foo_closure_t(x, y, z)));
}

int
DemoClient::fie(int8_t pos, float32_t gain)
{
  return (send(m_server, fie_closure_t(pos, gain)));
}

void
DemoClient::run()
{
  // Spread startup
  delay(rand(500, 5000));

  while (1) {
    // Some examples with different parameter types. Note size
    send(m_server, 0, 0);
    send(m_server, 1, 1, 2, 3, 0);
    send(m_server, 2, INT8_MAX, INT16_MAX, INT32_MAX, 0);
    send(m_server, 3, 'A', 'B', 'C', 0);
    send(m_server, 4, 1.25e8, M_PI, INFINITY, 0);
    send(m_server, 5, this, 0);

    // Pass a structure (copied on to stack)
    struct {
      int16_t x;
      int16_t y;
      int16_t z;
    } pos = { 1, 2, 3 };
    send(m_server, 6, pos, 0);

    // Pass a packed structure
    struct {
      uint8_t x;
      uint8_t y;
    } packed_pos = { 1, 2 };
    send(m_server, 7, packed_pos, 0);

    // Wrapper with hidden operation field
    foo(1, 2, 3);
    fie(10, M_PI);

    // Some possible methods for parameter count:
    // 1) Zero terminated list
    // 2) Counter/size prefix
    // 3) Counter/size encoded in operation code <op:count>
    // 4) Implicit by operation code
  }
}

DemoServer server;
DemoClient client1(&server);
DemoClient client2(&server);
DemoClient client3(&server);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNucleoActor: started"));
  Watchdog::begin();
  Nucleo::Thread::begin(&server, 128);
  Nucleo::Thread::begin(&client1, 196);
  Nucleo::Thread::begin(&client2, 196);
  Nucleo::Thread::begin(&client3, 196);
}

void loop()
{
  Nucleo::Thread::begin();
}
