/**
 * @file Cosa/RTC.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"

// Real-Time Clock configuration
#define COUNT 255
#define PRESCALE 64
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK ((COUNT + 1) * US_PER_TIMER_CYCLE)
#define TICKS_PER_SEC (1000000L / US_PER_TICK)

// Initiated state
bool RTC::s_initiated = false;

// Timer ticks counter
volatile uint32_t RTC::s_uticks = 0UL;
volatile uint16_t RTC::s_ticks = 0;
volatile uint32_t RTC::s_sec = 0L;

// Interrupt handler extension
RTC::InterruptHandler RTC::s_handler = NULL;
void* RTC::s_env = NULL;

bool
RTC::begin(InterruptHandler handler, void* env)
{
  if (s_initiated) return (false);
  synchronized {
    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));
    // And enable interrupt on overflow
    TIMSK0 = _BV(TOIE0);
    // Reset the counter and clear interrupts
    TCNT0 = 0;
    TIFR0 = 0;
  }
  s_handler = handler;
  s_env = env;
  s_initiated = true;
  return (true);
}

bool
RTC::end()
{
  // Disable the timer interrupts
  synchronized {
    TIMSK0 = 0;
  }
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
    if (TIFR0 & _BV(TOV0)) res += US_PER_TICK;
  }
  // Convert ticks to micro-seconds
  res += ((uint32_t) cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

void
RTC::delay(uint16_t ms, uint8_t mode)
{
  uint32_t start = RTC::millis();
  while (RTC::since(start) < ms) Power::sleep(mode);
}

ISR(TIMER0_OVF_vect)
{
  // Increment ticks and check for second count update
  uint16_t ticks = RTC::s_ticks;
  ticks += 1;
  if (ticks == TICKS_PER_SEC) {
    ticks = 0;
    RTC::s_sec += 1;
  }
  RTC::s_ticks = ticks;

  // Increment most significant part of micro second counter
  RTC::s_uticks += US_PER_TICK;
  
  if (RTC::Timer::s_queue_ticks > 0) {
    if (RTC::Timer::MEASURE)
      RTC::Timer::enter_ISR_cycle = TCNT0;
    // Decrement the most significant part of the RTC::Timer that's
    // expiring first. 
    RTC::Timer::s_queue_ticks--;
    if (RTC::Timer::s_queue_ticks == 0) {
      if (TCNT0 >= OCR0A) {
        // TCNT0 is already past the requested OCR0A time. Interrupt
        // disabling must have kept us from getting here in time. 
        TIFR0 |= _BV(TOV0); // RTC::micros() doesn't need to add US_PER_TICK any more
        RTC::Timer::schedule();
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
Head RTC::Timer::s_queue;
volatile uint32_t RTC::Timer::s_queue_ticks = 0;
volatile bool RTC::Timer::s_running = false;

#ifdef RTC_TIMER_MEASURE

uint8_t RTC::Timer::enter_setup_cycle        = 0;
uint8_t RTC::Timer::exit_setup_cycle         = 0;
uint8_t RTC::Timer::enter_start_cycle        = 0;
uint8_t RTC::Timer::enter_schedule_cycle     = 0;
uint8_t RTC::Timer::enter_ISR_cycle          = 0;
uint8_t RTC::Timer::enter_on_interrupt_cycle = 0;

#endif

/**
 * Measured timings, deduced from Timer0 cycles with 16MHz MCU clock*
 *
 * After making any code changes in RTC or RTC::Timer, you may want to re-measure
 * the times for various RTC::Timer operations by using a test program such as
 * CosaBenchmarkRTCTimer.ino.  The procedure is as follows:
 * (1) Reduce all xxx_US constants to 16 instructions.
 * (2) Rebuild and run a test program to obtain new values.
 * (3) Rebuild and verify that actual times are 0-2 timer cycles *greater*
 *     than the expiration times.  The actual time includes returning from 
 *     the interrupt and calling RTC::micros() for the stop time, so the
 *     actual time should always report taking a little longer than requested.
 */

/** Number of instructions from ::start to ::setup (queued dispatch) */
static const int32_t START_US = (320 / I_CPU);

/** Number of instructions from the beginning of ::setup to the end of ::setup */
static const int32_t SETUP_US = (128 / I_CPU);
  
/** Number of instructions from ISR to on_interrupt */
static const int32_t DISPATCH_US = ((272+64) / I_CPU);
  // added 1 cycle for int vectoring because TCNT0 is always OCR0A+1 at start of ISR

/** Visible constant computed from hidden constants */
const uint32_t
RTC::Timer::QUEUED_DISPATCH_TIME = (START_US + SETUP_US + DISPATCH_US);

void
RTC::Timer::setup(uint32_t us)
{
  if (MEASURE)
    enter_setup_cycle=TCNT0;

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

  if (MEASURE)
    exit_setup_cycle = TCNT0;
}

void
RTC::Timer::start()
{
  // Check if already queued
  if (is_started()) return;

  if (MEASURE)
    enter_start_cycle = TCNT0;

  // Not started yet. If this timer is getting *reStarted*, and we
  // are still within the ISR context, stack overflow would be
  // happening soon. Instead, put it in `s_queue` for later, even
  // though it might happen a little later than requested.
  int32_t us = m_expires - RTC::micros();
  bool immediate = (us <= (int32_t)QUEUED_DISPATCH_TIME);
  if (immediate) {
    us = 0;
    if (s_running)
      immediate = false;
  }
  if (!immediate) {
    bool first_changed = true;
    synchronized {
      Linkage* succ = &s_queue;
      Linkage* timer;
      while ((timer = succ->get_pred()) != &s_queue) {
        if (((RTC::Timer*) timer)->m_expires <= m_expires) {
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
RTC::Timer::stop()
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
RTC::Timer::schedule()
{
  if (MEASURE)
    enter_schedule_cycle = TCNT0;

  s_running = true;
  Linkage* timer;
  while ((timer = s_queue.get_succ()) != &s_queue) {
    int32_t us = ((RTC::Timer*) timer)->m_expires - RTC::micros();
    if (us >= (SETUP_US + DISPATCH_US)) {
      us -= DISPATCH_US;
      setup(us);
      break;
    }
    ((Link*) timer)->detach();
    ((RTC::Timer*) timer)->on_expired();
  }
  s_running = false;
}

ISR(TIMER0_COMPA_vect)
{
  if (RTC::Timer::MEASURE)
    RTC::Timer::enter_ISR_cycle = TCNT0;

  TIMSK0 &= ~_BV(OCIE0A);
  RTC::Timer::schedule();
}
