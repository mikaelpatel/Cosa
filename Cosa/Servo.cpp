/**
 * @file Cosa/Servo.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * @section Description
 * Servo motor driver. Uses Timer1 and the two compare output
 * registers (OCR1A/B).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Servo.hh"

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
  return (1);
}

bool 
Servo::end()
{
  TIMSK1 &= ~_BV(OCIE1A);
  TIMSK1 &= ~_BV(OCIE1B);
  return (1);
}

void 
Servo::set_pos(uint16_t angle) 
{
  if (angle > 180) angle = 180;
  uint16_t width = (((uint32_t) (m_max - m_min)) * angle) / 180L;
  synchronized {
    m_width = m_min + width;
    m_angle = angle;
  }
}  

ISR(TIMER1_COMPA_vect) 
{ 
  Servo* servo = Servo::servo[0];
  if (servo == 0) return;
  servo->toggle();
  if (servo->is_set())
    OCR1A = TCNT1 + US_TO_TICKS(servo->m_width);
  else
    OCR1A = TCNT1 + US_TO_TICKS(Servo::PERIOD - servo->m_width);
}

ISR(TIMER1_COMPB_vect) 
{ 
  Servo* servo = Servo::servo[1];
  if (servo == 0) return;
  servo->toggle();
  if (servo->is_set())
    OCR1B = TCNT1 + US_TO_TICKS(servo->m_width);
  else
    OCR1B = TCNT1 + US_TO_TICKS(Servo::PERIOD - servo->m_width);
}
