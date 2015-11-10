/**
 * @file Cosa/Tone.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel (Cosa C++ port and extensions)
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

#include "Cosa/Tone.hh"
#include "Cosa/Watchdog.hh"

#if !defined(BOARD_ATTINY)
#if defined (__AVR_ATmega32U4__)		\
  || defined(__AVR_ATmega640__)			\
  || defined(__AVR_ATmega1280__)		\
  || defined(__AVR_ATmega1281__)		\
  || defined(__AVR_ATmega2560__)		\
  || defined(__AVR_ATmega2561__)
#define PWM1 DDB5
#define PWM2 DDB6
#define DDR DDRB
#define PORT PORTB
#elif defined(__AVR_ATmega1284P__)		\
  || defined(__AVR_ATmega644__)			\
  || defined(__AVR_ATmega644P__)
#define PWM1 DDD4
#define PWM2 DDD5
#define DDR DDRD
#define PORT PORTD
#elif defined(__AVR_ATmega256RFR2__)
#define PWM1 DDRB5
#define PWM2 DDRB6
#define DDR DDRB
#define PORT PORTB
#else
#define PWM1 DDB1
#define PWM2 DDB2
#define DDR DDRB
#define PORT PORTB
#endif

uint32_t Tone::s_expires;
const uint8_t Tone::s_map[] __PROGMEM = {
  200, 100, 67, 50, 40, 33, 29, 22, 11, 2
};

void
Tone::begin()
{
  // Initiate PWM pins as output
  DDR |= (_BV(PWM1) | _BV(PWM2));
}

void
Tone::play(uint16_t freq, uint8_t volume, uint16_t duration, bool background)
{
  // Check if turn off tone
  if (UNLIKELY((freq == 0) || (volume == 0))) {
    silent();
    return;
  }

  // Check volume does not exceed limit
  if (UNLIKELY(volume > VOLUME_MAX)) volume = VOLUME_MAX;

  // Calculate clock prescaling
  Power::timer1_enable();
  uint8_t prescaler = _BV(CS10);
  uint32_t top = (F_CPU / freq / 2) - 1;
  if (top > 65535L) {
    prescaler = _BV(CS12);
    top = (top / 256) - 1;
  }

  // Get duty from volume map
  uint16_t duty = top / pgm_read_byte(&s_map[volume - 1]);

  // Check if interrupt handler should be enabled to turn off tone
  if ((duration > 0) && background) {
    s_expires = Watchdog::millis() + duration;
    TIMSK1 |= _BV(OCIE1A);
  }
  ICR1 = top;
  if (TCNT1 > top) TCNT1 = top;
  TCCR1B = (_BV(WGM13) | prescaler);
  OCR1A = duty;
  OCR1B = duty;
  TCCR1A = (_BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0));

  // Check for asychronious mode
  if ((duration == 0) || background) return;
  delay(duration);
  silent();
}

void
Tone::silent()
{
  // Turn off interrupt handler
  TIMSK1 &= ~_BV(OCIE1A);
  TCCR1B = _BV(CS11);
  TCCR1A = _BV(WGM10);

  // Clear output pin
  PORT &= ~(_BV(PIN1) | _BV(PIN2));
  Power::timer1_disable();
}

ISR(TIMER1_COMPA_vect)
{
  // Check if the tone should be turned off
  if (Watchdog::millis() < Tone::s_expires) return;
  Tone::silent();
}

#endif

