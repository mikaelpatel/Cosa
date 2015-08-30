/**
 * @file CosaTimer.ino
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
 * Study behavior of RTC Timer auxilary match register interrupt
 * handling for micro-second application level timer.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"

OutputPin led(Board::LED);

volatile int16_t cycles;

void on_expire(void* env)
{
  cycles += 1;
  if (env != NULL) led.toggle();
}

void sync()
{
  uint32_t ms = RTC::millis();
  while (ms == RTC::millis());
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTimer: started"));
  trace.flush();
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  uint16_t US[] = { 50, 100, 200, 500, 700, 900, 999 };
  uint16_t start;
  uint16_t stop;
  uint16_t us;

  for (uint8_t i = 0; i < membersof(US); i++) {
    sync();
    led.on();
    start = RTC::micros();
    RTC::expire_in(US[i], on_expire);
    while (!RTC::is_expired());
    led.off();
    stop = RTC::micros();
    us = stop - start;
    trace << PSTR("US[i]=") << US[i]
	  << PSTR(",us=") << us
	  << PSTR(":") << us - US[i]
	  << endl;
    trace.flush();
    delay(1000);
  }

  for (uint8_t i = 0; i < membersof(US); i++) {
    int16_t CYCLES = 10000 / US[i];
    cycles = 0;
    sync();
    led.on();
    RTC::periodic_start(US[i], on_expire, &CYCLES);
    delay(10);
    RTC::periodic_stop();
    led.off();
    trace << PSTR("CYCLES[i]=") << CYCLES
	  << PSTR(",cycles=") << cycles
	  << PSTR(":") << cycles - CYCLES
	  << endl;
    trace.flush();
    delay(1000);
  }
}
