/**
 * @file Cosa/Tone.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel (C++ refactoring and extensions)
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
#define PWMT1AMASK DDB5
#define PWMT1BMASK DDB6
#define PWMT1DREG DDRB
#define PWMT1PORT PORTB
#elif defined(__AVR_ATmega1284P__)		\
  || defined(__AVR_ATmega644__)			\
  || defined(__AVR_ATmega644P__)
#define PWMT1AMASK DDD4
#define PWMT1BMASK DDD5
#define PWMT1DREG DDRD
#define PWMT1PORT PORTD
#else
#define PWMT1AMASK DDB1
#define PWMT1BMASK DDB2
#define PWMT1DREG DDRB
#define PWMT1PORT PORTB
#endif

uint32_t Tone::s_time;
const uint8_t Tone::s_map[] __PROGMEM = { 
  200, 100, 67, 50, 40, 33, 29, 22, 11, 2 
};

void 
Tone::begin()
{
  // Initiate PWM pins as output
  PWMT1DREG |= (_BV(PWMT1AMASK) | _BV(PWMT1BMASK));
}

void 
Tone::play(uint16_t freq, uint8_t volume, uint16_t duration, bool background) 
{
  // Check if turn off tone
  if ((freq == 0) || (volume == 0)) { 
    silent();
    return;
  }

  // Check volume does not exceed limit
  if (volume > VOLUME_MAX) volume = VOLUME_MAX;
  
  // Calculate clock prescaling 
  uint8_t prescaler = _BV(CS10);
  unsigned long top = (F_CPU / freq / 2) - 1;
  if (top > 65535) {
    prescaler = _BV(CS12);
    top = top / 256 - 1;
  }

  // Get duty from volume map
  uint16_t duty = top / pgm_read_byte(&s_map[volume - 1]);

  // Check if interrupt handler should be enabled to turn off tone
  if ((duration > 0) && background) {
    s_time = Watchdog::millis() + duration;
    TIMSK1 |= _BV(OCIE1A);
  }
  ICR1 = top;
  if (TCNT1 > top) TCNT1 = top;
  TCCR1B = (_BV(WGM13)  | prescaler);
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
  PWMT1PORT &= ~(_BV(PWMT1AMASK) | _BV(PWMT1BMASK));
}

ISR(TIMER1_COMPA_vect) 
{
  // Check if the tone should be turned off
  if (Watchdog::millis() < Tone::s_time) return;
  Tone::silent();
}

#endif

