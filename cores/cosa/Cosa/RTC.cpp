/**
 * @file Cosa/RTC.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/GPIO.hh"

// Real-Time Clock configuration
#define COUNT 250
#define PRESCALE 64
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK (COUNT * US_PER_TIMER_CYCLE)
#define US_PER_MS 1000
#define MS_PER_SEC 1000
#define MS_PER_TICK (US_PER_TICK / 1000)

// Initiated state
bool RTC::s_initiated = false;

// Timer ticks counter
volatile uint32_t RTC::s_uticks = 0UL;

// Milli-seconds counter
volatile uint32_t RTC::s_ms = 0UL;

// Seconds counter with milli-seconds fraction
volatile uint16_t RTC::s_msec = 0;
volatile clock_t RTC::s_sec = 0UL;

// Timer interrupt extension
RTC::InterruptHandler RTC::s_handler = NULL;
void* RTC::s_env = NULL;

bool
RTC::begin()
{
  if (UNLIKELY(s_initiated)) return (false);
  synchronized {
    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));

    // Clear Timer on Compare Match
    TCCR0A = _BV(WGM01);
    OCR0A = COUNT;

    // And enable interrupt on match
    TIMSK0 = _BV(OCIE0A);

    // Reset the counter and clear interrupts
    TCNT0 = 0;
    TIFR0 = 0;
  }

  // Install delay function and mark as initiated
  ::delay = RTC::delay;
  s_initiated = true;

  return (true);
}

bool
RTC::end()
{
  // Check if initiated
  if (UNLIKELY(!s_initiated)) return (false);

  // Disable the timer interrupts and mark as not initiated
  synchronized TIMSK0 = 0;
  s_initiated = false;

  return (true);
}

uint16_t
RTC::us_per_tick()
{
  return (US_PER_TICK);
}

uint16_t
RTC::us_per_timer_cycle()
{
  return (US_PER_TIMER_CYCLE);
}

uint32_t
RTC::micros()
{
  uint32_t res;
  uint8_t cnt;

  // Read tick count and hardware counter. Adjust if pending interrupt
  synchronized {
    res = s_uticks;
    cnt = TCNT0;
    if ((TIFR0 & _BV(OCF0A)) && cnt < COUNT) res += US_PER_TICK;
  }

  // Convert ticks to micro-seconds
  res += ((uint32_t) cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

void
RTC::delay(uint32_t ms)
{
  uint32_t start = RTC::millis();
  while (RTC::since(start) < ms) yield();
}

int
RTC::await(volatile bool &condvar, uint32_t ms)
{
  if (ms != 0) {
    uint32_t start = millis();
    while (!condvar && since(start) < ms) yield();
    if (UNLIKELY(!condvar)) return (ETIME);
  }
  else {
    while (!condvar) yield();
  }
  return (0);
}

#if defined(BOARD_ATTINYX4)
#define PIN Board::D7
#elif defined(BOARD_ATTINYX5)
#define PIN Board::D0
#elif defined(BOARD_ATMEGA328P)
#define PIN Board::D6
#elif defined(BOARD_ATMEGA1248P)
#define PIN Board::D3
#elif defined(BOARD_ATMEGA1280)
#define PIN Board::D13
#elif defined(BOARD_ATMEGA2560)
#define PIN Board::D13
#elif defined(BOARD_ATMEGA32U4)
#define PIN Board::D11
#endif

void
RTC::enable()
{
#if defined(PIN)
  TCCR0A |= _BV(COM0A0);
  GPIO::set_mode(PIN, GPIO::OUTPUT_MODE);
#endif
}

void
RTC::disable()
{
#if defined(PIN)
  TCCR0A &= ~_BV(COM0A0);
  GPIO::set_mode(PIN, GPIO::INPUT_MODE);
#endif
}

ISR(TIMER0_COMPA_vect)
{
  // Increment most significant part of micro seconds counter
  RTC::s_uticks += US_PER_TICK;

  // Increment milli-seconds counter
  RTC::s_ms += MS_PER_TICK;

  // Increment milli-seconds fraction of seconds counter
  RTC::s_msec += MS_PER_TICK;

  // Check for increment of seconds counter
  if (UNLIKELY(RTC::s_msec >= MS_PER_SEC)) {
    RTC::s_msec -= MS_PER_SEC;
    RTC::s_sec += 1;
  }

  // Check for extension of the interrupt handler
  RTC::InterruptHandler fn = RTC::s_handler;
  void* env = RTC::s_env;
  if (UNLIKELY(fn == NULL)) return;

  // Callback to extension function
  fn(env);
}
