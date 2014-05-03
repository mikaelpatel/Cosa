/**
 * @file Cosa/Timer.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Timer.hh"
#include "Cosa/Power.hh"

// Real-Time Clock configuration
#define COUNT 255
#define PRESCALE 64
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK ((COUNT + 1) * US_PER_TIMER_CYCLE)
#define TICKS_PER_SEC (1000000L / US_PER_TICK)

void
Timer::on_interrupt(void* env)
{
  UNUSED(env);
  if (Timer::s_queue_ticks > 0) {
    if (Timer::MEASURE) Timer::enter_ISR_cycle = TCNT0;
    // Decrement the most significant part of the Timer that's
    // expiring first. 
    Timer::s_queue_ticks--;
    if (Timer::s_queue_ticks == 0) {
      if (TCNT0 >= OCR0A) {
        // TCNT0 is already past the requested OCR0A time. Interrupt
        // disabling must have kept us from getting here in time. 
	// RTC::micros() doesn't need to add US_PER_TICK any more
        TIFR0 |= _BV(TOV0);
        Timer::schedule();
      } else {
        // The remaining time will be caught by OCR0A. Clear the
        // previous match, because the correct one will be here soon.
        TIFR0 |= _BV(OCF0A);
        TIMSK0 |= _BV(OCIE0A);
      }
    }
  }
}

// Timer queue and state
Head Timer::s_queue;
volatile uint32_t Timer::s_queue_ticks = 0;
volatile bool Timer::s_running = false;

// Measurement static members
uint8_t Timer::enter_setup_cycle;
uint8_t Timer::exit_setup_cycle;
uint8_t Timer::enter_start_cycle;
uint8_t Timer::enter_schedule_cycle;
uint8_t Timer::enter_ISR_cycle;
uint8_t Timer::enter_on_interrupt_cycle;

/**
 * Measured timings, deduced from Timer0 cycles with 16MHz MCU clock*
 *
 * After making any code changes in RTC or Timer, you may want to re-measure
 * the times for various Timer operations by using a test program such as
 * CosaBenchmarkRTCTimer.ino.  The procedure is as follows:
 * (1) Reduce all xxx_US constants to 16 instructions.
 * (2) Rebuild and run a test program to obtain new values.
 * (3) Rebuild and verify that actual times are 0-2 timer cycles *greater*
 *     than the expiration times.  The actual time includes returning from 
 *     the interrupt and calling micros() for the stop time, so the
 *     actual time should always report taking a little longer than requested.
 */

/** Number of instructions from ::start to ::setup (queued dispatch) */
static const int32_t START_US = (320 / I_CPU);

/** Number of instructions from the beginning of ::setup to the end of ::setup */
static const int32_t SETUP_US = (128 / I_CPU);
  
/** 
 * Number of instructions from ISR to on_interrupt. Added 1 cycle for
 * int vectoring because TCNT0 is always OCR0A+1 at start of ISR 
 */
static const int32_t DISPATCH_US = ((272+64) / I_CPU);

/** Visible constant computed from hidden constants */
const uint32_t
Timer::QUEUED_DISPATCH_TIME = (START_US + SETUP_US + DISPATCH_US);

void
Timer::setup(uint32_t us)
{
  if (MEASURE) enter_setup_cycle=TCNT0;

  uint32_t timer_cycles  = us / US_PER_TIMER_CYCLE;
  if (timer_cycles >= 256) {
    timer_cycles += TCNT0;
    TIMSK0 &= ~_BV(OCIE0A);
    OCR0A = (uint8_t) timer_cycles;
    TIFR0 |=  _BV(OCF0A);
    s_queue_ticks = timer_cycles >> 8;
    return;
  } 

  // For the case where timer_cycles == 0, there is a small chance 
  // that TCNT0 could advance /after/ its value has been read and 
  // added to timer_cycles, which is then written to OCR0A.
  // Then the OC interrupt would *not* trigger immediately as
  // requested. Instead, 255 cycles would have to elapse.  There seems
  // to be two choices: 
  // (1) Stop the timer while we're messing with the registers; or
  // (2) Add one to a timer_cycle of 0 so that a match will happen
  // *soon*, but perhaps not immediately as requested.
  // Let's do the latter so we don't lose the occasional RTC tick.
  if (timer_cycles == 0)
    timer_cycles = 1;

  TIFR0 |= _BV(OCF0A);
  OCR0A  = TCNT0 + (uint8_t) timer_cycles;
  if (OCR0A == 0) {
    // Catch it in TIMER0_OVF
    TIMSK0 &= ~_BV(OCIE0A);
    s_queue_ticks = 1;
  } 
  else {
    // Catch it in TIMER0_OCR0A
    TIMSK0 |= _BV(OCIE0A);
    s_queue_ticks = 0;
  }
  if (MEASURE) exit_setup_cycle = TCNT0;
}

void
Timer::start()
{
  // Check if already queued
  if (is_started()) return;

  // Not started yet. If this timer is getting *reStarted*, and we
  // are still within the ISR context, stack overflow would be
  // happening soon. Instead, put it in `s_queue` for later, even
  // though it might happen a little later than requested.
  if (MEASURE) enter_start_cycle = TCNT0;
  int32_t us = m_expires - RTC::micros();
  bool immediate = (us <= (int32_t)QUEUED_DISPATCH_TIME);
  if (immediate) {
    us = 0;
    if (s_running)
      immediate = false;
  }
  if (!immediate) {
    bool first_changed = true;
    m_expires = RTC::micros() + us;
    synchronized {
      Linkage* succ = &s_queue;
      Linkage* timer;
      while ((timer = succ->get_pred()) != &s_queue) {
        if (((Timer*) timer)->m_expires <= m_expires) {
          first_changed = false;
          break;
        }
        succ = timer;
      }
      succ->attach(this);
      if (first_changed) {
        us = m_expires - RTC::micros();
        if (us >= (SETUP_US + DISPATCH_US))
          us -= DISPATCH_US;
        else
          us = 0;
        setup(us);
      }
    }
  } 
  else {
    bool saved = s_running;
    s_running = true;
    on_expired();
    s_running = saved;
  }
}

void
Timer::stop()
{
  if (!is_started()) return;
  synchronized {
    bool was_first = (get_pred() == &s_queue);
    detach();
    if (was_first)
      schedule();
  }
}

void
Timer::schedule()
{
  if (MEASURE) enter_schedule_cycle = TCNT0;
  s_running = true;
  Linkage* timer;
  while ((timer = s_queue.get_succ()) != &s_queue) {
    int32_t us = ((Timer*) timer)->m_expires - RTC::micros();
    if (us >= (SETUP_US + DISPATCH_US)) {
      us -= DISPATCH_US;
      setup(us);
      break;
    }
    ((Link*) timer)->detach();
    ((Timer*) timer)->on_expired();
  }
  s_running = false;
}

ISR(TIMER0_COMPA_vect)
{
  if (Timer::MEASURE) Timer::enter_ISR_cycle = TCNT0;
  TIMSK0 &= ~_BV(OCIE0A);
  Timer::schedule();
}

