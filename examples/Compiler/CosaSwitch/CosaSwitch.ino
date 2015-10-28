/**
 * @file CosaSwitch.ino
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
 * Testing code generation of switch statements.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

enum {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_REM,
  OP_LOAD,
  OP_STORE,
  OP_BRA,
  OP_BRZ,
  OP_CALL,
  OP_EXIT
};

// Traditional switch statement will compile to if-else if
#define DISPATCH(op) switch (op)
#define OP(x) case OP_ ## x:
int switch_fn(uint8_t op)
{
  int res = 0;
  DISPATCH(op) {
    OP(ADD)
      res = 1;
    break;
    OP(SUB)
      res = 2;
    break;
    OP(MUL)
      res = 3;
    break;
    OP(DIV)
      res = 4;
    break;
    OP(REM)
      res = 5;
    break;
    OP(LOAD)
      res = 6;
    break;
    OP(STORE)
      res = 7;
    break;
    OP(BRA)
      res = 8;
    break;
    OP(BRZ)
      res = 9;
    break;
    OP(CALL)
      res = 10;
    break;
    OP(EXIT)
      res = 11;
    break;
  }
  return (res);
}
#undef OP
#undef DISPATCH

// Table driven switch statement using gcc goto extension and program memory table
#define OP(x) DO_ ## x:
#define DISPATCH(op) goto *pgm_read_word(&tab[op]); for (uint8_t i = 0; i < 1; i++)
int goto_fn(uint8_t op)
{
  // Label address table
  static void* const tab[] __PROGMEM = {
    &&DO_ADD,
    &&DO_SUB,
    &&DO_MUL,
    &&DO_DIV,
    &&DO_REM,
    &&DO_LOAD,
    &&DO_STORE,
    &&DO_BRA,
    &&DO_BRZ,
    &&DO_CALL,
    &&DO_EXIT
  };
  int res = 0;

  // Switch implementation with table and do-while(0) loop to allow break
  DISPATCH(op) {
    OP(ADD)
      res = 1;
    break;
    OP(SUB)
      res = 2;
    break;
    OP(MUL)
      res = 3;
    break;
    OP(DIV)
      res = 4;
    break;
    OP(REM)
      res = 5;
    break;
    OP(LOAD)
      res = 6;
    break;
    OP(STORE)
      res = 7;
    break;
    OP(BRA)
      res = 8;
    break;
    OP(BRZ)
      res = 9;
    break;
    OP(CALL)
      res = 10;
    break;
    OP(EXIT)
      res = 11;
    break;
  }
  return (res);
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSwitch: started"));
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  uint32_t start, stop, us;
  uint8_t* switch_fn_addr = (uint8_t*) switch_fn;
  uint8_t* goto_fn_addr = (uint8_t*) goto_fn;
  uint8_t* loop_addr = (uint8_t*) loop;
  size_t switch_fn_size = goto_fn_addr - switch_fn_addr;
  size_t goto_fn_size = loop_addr - goto_fn_addr;
  int res;

  // Measure traditional switch statement version
  TRACE(switch_fn_addr);
  TRACE(switch_fn_size);
  for (uint8_t op = OP_ADD; op <= OP_EXIT; op++)
    TRACE(switch_fn(op));
  sleep(1);

  start = RTT::micros();
  res = 0;
  for (int i = 0; i < 1000; i++)
    for (uint8_t op = OP_ADD; op <= OP_EXIT; op++)
      res += switch_fn(op);
  stop = RTT::micros();
  us = stop - start;
  TRACE(res);
  TRACE(us);
  sleep(1);

  start = RTT::micros();
  for (int i = 0; i < 10000; i++)
    for (uint8_t op = OP_LOAD; op <= OP_EXIT; op++)
      res += switch_fn(op);
  stop = RTT::micros();
  us = stop - start;
  TRACE(res);
  TRACE(us);
  sleep(1);

  // Measure goto-label table version
  TRACE(goto_fn_addr);
  TRACE(goto_fn_size);
  for (uint8_t op = OP_ADD; op <= OP_EXIT; op++)
    TRACE(goto_fn(op));
  sleep(1);

  start = RTT::micros();
  res = 0;
  for (int i = 0; i < 1000; i++)
    for (uint8_t op = OP_ADD; op <= OP_EXIT; op++)
      res += goto_fn(op);
  stop = RTT::micros();
  us = stop - start;
  TRACE(res);
  TRACE(us);
  sleep(1);

  start = RTT::micros();
  for (int i = 0; i < 10000; i++)
    for (uint8_t op = OP_LOAD; op <= OP_EXIT; op++)
      res += goto_fn(op);
  stop = RTT::micros();
  us = stop - start;
  TRACE(res);
  TRACE(us);
  sleep(1);

  ASSERT(true == false);
}

