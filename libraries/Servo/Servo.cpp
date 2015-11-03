/**
 * @file Servo.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#include "Cosa/Board.hh"
#if !defined(BOARD_ATTINYX5)
#include "Servo.hh"

Servo* Servo::servo[2] = { 0, 0 };

#define US_TO_TICKS(us) ((I_CPU * (us)) / 8)

bool
Servo::begin()
{
  TCCR1A = 0;
  TCCR1B = _BV(CS11);
  TCNT1 = 0;
  TIMSK1 |= _BV(OCIE1A);
  TIMSK1 |= _BV(OCIE1B);
  OCR1A = TCNT1 + PERIOD;
  OCR1B = TCNT1 + PERIOD;
  return (true);
}

bool
Servo::end()
{
  TIMSK1 &= ~_BV(OCIE1A);
  TIMSK1 &= ~_BV(OCIE1B);
  return (true);
}

void
Servo::angle(uint8_t degree)
{
  if (UNLIKELY(degree > 180)) degree = 180;
  uint16_t width = (((uint32_t) (m_max - m_min)) * degree) / 180L;
  synchronized {
    m_width = m_min + width;
    m_angle = degree;
  }
}

ISR(TIMER1_COMPA_vect)
{
  Servo* servo = Servo::servo[0];
  if (UNLIKELY(servo == NULL)) return;
  servo->toggle();
  if (servo->is_set())
    OCR1A = TCNT1 + US_TO_TICKS(servo->m_width);
  else
    OCR1A = TCNT1 + US_TO_TICKS(Servo::PERIOD - servo->m_width);
}

ISR(TIMER1_COMPB_vect)
{
  Servo* servo = Servo::servo[1];
  if (UNLIKELY(servo == NULL)) return;
  servo->toggle();
  if (servo->is_set())
    OCR1B = TCNT1 + US_TO_TICKS(servo->m_width);
  else
    OCR1B = TCNT1 + US_TO_TICKS(Servo::PERIOD - servo->m_width);
}

#endif
