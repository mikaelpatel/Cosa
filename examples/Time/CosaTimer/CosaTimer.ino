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
#include "Cosa/OutputPin.hh"
#include "Cosa/IOStream/Driver/UART.hh"

OutputPin led(Board::LED);
volatile int16_t cycles;

void on_expire(void* env)
{
  UNUSED(env);
  led.off();
}

void on_period(void* env)
{
  UNUSED(env);
  cycles += 1;
  led.toggle();
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTimer: started"));
  trace.flush();
  RTC::begin();
  RTC::enable_pin_toggle();
  uint32_t ms = RTC::millis();
  while (ms == RTC::millis());
}

void loop()
{
  uint16_t start;
  uint16_t stop;
  uint16_t us;

  led.on();
  sleep(2);
  led.off();

  for (uint16_t US = 80; US < 1000; US += 100) {
    RTC::expire_in(US, on_expire);
    start = RTC::micros();
    led.on();
    while (!RTC::is_expired());
    led.off();
    stop = RTC::micros();
    us = stop - start;
    trace << PSTR("US=") << US
	  << PSTR(",us=") << us
	  << PSTR(":") << us - US
	  << endl;
    trace.flush();
    delay(1000);
  }

  for (uint16_t US = 60; US < 1000; US += 100) {
    int16_t CYCLES = 10000 / US;
    cycles = 0;
    led.on();
    RTC::periodic_start(US, on_period);
    delay(10);
    RTC::periodic_stop();
    led.off();
    trace << PSTR("CYCLES=") << CYCLES
	  << PSTR(",cycles=") << cycles
	  << PSTR(":") << cycles - CYCLES
	  << endl;
    trace.flush();
    delay(1000);
  }
}
