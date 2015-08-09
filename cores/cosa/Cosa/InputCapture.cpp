/**
 * @file Cosa/InputCapture.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/InputCapture.hh"

#if !defined(BOARD_ATTINY)

InputCapture* InputCapture::s_capture = NULL;

InputCapture::InputCapture(InterruptMode mode)
{
  // This is the active input capture
  s_capture = this;

  // Use no prescale; timer runs at system clock frequency
  TCCR1B = _BV(CS10);

  // Set trigger on rising or falling on input capture pin
  if (mode == ON_RISING_MODE)
    TCCR1B |= _BV(ICES1);
  else
    TCCR1B &= ~_BV(ICES1);
}

void
InputCapture::on_interrupt(uint16_t arg)
{
  UNUSED(arg);
}

void
InputCapture::enable()
{
  // Enable input capture interrupt
  TIMSK1 |= _BV(ICIE1);
}

void
InputCapture::disable()
{
  // Disable input capture interrupt
  TIMSK1 &= ~_BV(ICIE1);
}

void
InputCapture::clear()
{
  // Clear input capture interrupt flag if set
  TIFR1 = _BV(ICF1);
}

ISR(TIMER1_CAPT_vect)
{
  if (UNLIKELY(InputCapture::s_capture == NULL)) return;
  InputCapture::s_capture->on_interrupt(ICR1);
}
#endif
