/**
 * @file Cosa/RTCMeasure.cpp
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

#include "Cosa/RTCMeasure.hh"

#ifdef RTC_TIMER_MEASURE

uint8_t RTCMeasure::start_immediate_cycles = 0;
uint8_t RTCMeasure::start_queued_cycles    = 0;
uint8_t RTCMeasure::setup_cycles           = 0;
uint8_t RTCMeasure::dispatch_cycles        = 0;

const uint16_t RTCMeasure::I_PER_CYCLE = (RTC::us_per_timer_cycle() * I_CPU);

void
RTCMeasure::start()
{
  m_dispatched = false;

  enter_setup_cycle        = 0;
  exit_setup_cycle         = 0;
  enter_start_cycle        = 0;
  enter_schedule_cycle     = 0;
  enter_ISR_cycle          = 0;
  enter_on_interrupt_cycle = 0;

  start_immediate_cycles = 0;
  start_queued_cycles    = 0;
  setup_cycles           = 0;
  dispatch_cycles        = 0;
  
  RTC::Timer::start();

  if ((enter_on_interrupt_cycle != 0) ||
      ((enter_setup_cycle == 0) && (exit_setup_cycle == 0)))
    // It was an immediate dispatch
    start_immediate_cycles = enter_on_interrupt_cycle - enter_start_cycle + 1;
  else {
    // It was queued up and dispatched by an interrupt
    start_queued_cycles = enter_setup_cycle - enter_start_cycle + 1;
    setup_cycles        = exit_setup_cycle  - enter_setup_cycle + 1;
  }
}

void
RTCMeasure::on_expired()
{
  enter_on_interrupt_cycle = TCNT0;

  dispatch_cycles = enter_on_interrupt_cycle - enter_ISR_cycle + 1;
  
  m_dispatched = true;
}

#endif