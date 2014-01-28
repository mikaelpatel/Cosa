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

uint32_t
RTC::micros()
{
  uint32_t res;
  uint8_t cnt;
  // Read tick count and hardware counter. Adjust if pending interrupt
  synchronized {
    res = s_uticks;
    cnt = TCNT0;
    if ((TIFR0 & _BV(TOV0)) && cnt < COUNT) res += US_PER_TICK;
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
  
  if (RTC::Timer::queue_ticks > 0) {
    // Decrement the most significant part of the
    //    RTC::Timer that's expiring first.
    RTC::Timer::queue_ticks--;

    if (RTC::Timer::queue_ticks == 0) {

      if (TCNT0 >= OCR0A) {
        //  TCNT0 is already past the requested OCR0A time.
        //    Interrupt disabling must have kept us from getting here in time.
        RTC::Timer::check_queue();
      } else {
        //  The remaining time will be caught by OCR0A.  Clear the previous
        //    match, because the correct one will be here soon.
        TIFR0  |= _BV(OCF0A);
        TIMSK0 |= _BV(OCIE0A);
      }
    }
  }
}

//-------------------------------------

         Head    RTC::Timer::queue;
volatile uint8_t RTC::Timer::queue_ticks = 0;
volatile bool    RTC::Timer::in_ISR      = false;

void
RTC::Timer::setup( uint32_t uS )
{
  uint32_t timer_cycles  = uS / US_PER_TIMER_CYCLE;

  if (timer_cycles >= 256) {
    timer_cycles += TCNT0;
    OCR0A   = (uint8_t) timer_cycles;
    TIMSK0 &= ~_BV(OCIE0A);
    TIFR0  |=  _BV(OCF0A);
    queue_ticks = timer_cycles >> 8;

  } else {
    //  For the case where timer_cycles == 0, there is a small chance 
    //    that TCNT0 could advance /after/ its value has been read and 
    //    added to timer_cycles, which is then written to OCR0A.
    //  Then the OC interrupt would *not* trigger immediately as requested.
    //  Instead, 255 cycles would have to elapse.  There seems to be two choices:
    //     (1) Stop the timer while we're messing with the registers; or
    //     (2) Add one to a timer_cycle of 0 so that a match will happen *soon*,
    //            but perhaps not immediately as requested.
    //  Let's do the latter so we don't lose the occasional RTC tick.
    if (timer_cycles == 0)
      timer_cycles = 1;

    TIFR0 |= _BV(OCF0A);
    OCR0A  = TCNT0 + (uint8_t) timer_cycles;
    if (OCR0A == 0)
      // catch it in TIMER0_OVF
      queue_ticks = 1;
    else {
      // catch it in TIMER0_OCR0A
      TIMSK0 |= _BV(OCIE0A);
      queue_ticks = 0;
    }
  }


} // setup

//------------------------------------------------

void
RTC::Timer::start()
{
  if (get_pred() == this) {
    // Not started yet

    // If this timer is getting *reStarted*, and we are still within
    //    the ISR context, stack overflow would be happening soon.  :(
    // Instead, put it in `queue` for later, even though it might
    //    happen a little later than requested.

    int32_t  uS    = expiration - RTC::micros();
    bool     immed = (uS <= US_PER_TIMER_CYCLE);

    if (immed) {
      uS = 0;
      if (in_ISR)
        immed = false;
    }

    if (!immed) {
      bool first_changed = true;

      synchronized {
        // Insert 'this' in the right spot...
        Linkage *succ      = &queue;
        bool     was_empty = queue.is_empty();

        if (!was_empty) {
          Linkage *timer = succ->get_pred(); // start at the end

          do {
            if (((RTC::Timer *)timer)->expiration <= expiration) {
              first_changed = false;
              break;
            }

            succ  = timer;
            timer = succ->get_pred();
          } while (timer != &queue);
        }

        succ->attach( this );

        if (first_changed) {
          // That took some time...
          uS = expiration - RTC::micros();

          setup( uS );
        }
      } // synchronized

    } else {
      // Immediate expiration

      bool oldISR = in_ISR;
      in_ISR = true; // Maybe not, but it might prevent infinite recursion
      on_interrupt();
      in_ISR = oldISR;
    }
  }
} // start

//----------------------------------------------

void
RTC::Timer::check_queue()
{
  in_ISR = true;
  while (true) {
    Linkage *timer = queue.get_succ();

    if (timer == &queue) // empty
      break;

    int32_t uS = ((RTC::Timer *)timer)->expiration - RTC::micros();
    if (uS > 0) {
      setup( uS );
      break;
    }

    ((Link       *)timer)->detach();
    ((RTC::Timer *)timer)->on_interrupt();
  }
  in_ISR = false;

} // check_queue

//----------------------------------------------

ISR(TIMER0_COMPA_vect)
{
  TIMSK0 &= ~_BV(OCIE0A);

  RTC::Timer::check_queue();

} // TIMER0_COMPA_vect
