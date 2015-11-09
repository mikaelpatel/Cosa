/**
 * @file Cosa/PWMPin.cpp
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

#include "Cosa/PWMPin.hh"
#include "Cosa/Power.hh"

#if defined(BOARD_ATMEGA328P)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) :
  OutputPin((Board::DigitalPin) pin)
{
  switch (pin) {
  case Board::PWM1:
  case Board::PWM2:
    // PWM1(0B), PMW2(0A), Fast PWM, prescale 64
    TCCR0A |= _BV(WGM01) | _BV(WGM00);
    TCCR0B |= _BV(CS01) | _BV(CS00);
    break;

  case Board::PWM3:
  case Board::PWM4:
    // PWM3(1A), PWM4(1B), PWM phase correct, 8-bit, prescale 64
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(CS11) | _BV(CS10);
    break;

  case Board::PWM0:
  case Board::PWM5:
    // PWM0(2B), PWM5(2A), PWM phase correct, prescale 64
    TCCR2A |= _BV(WGM20);
    TCCR2B |= _BV(CS22);
    break;
  }
  set(duty);
}

void
PWMPin::begin()
{
  switch (m_pin) {
  case Board::PWM0:
  case Board::PWM5:
    Power::timer2_enable();
    break;
  case Board::PWM1:
  case Board::PWM2:
    Power::timer0_enable();
    break;
  case Board::PWM3:
  case Board::PWM4:
    Power::timer1_enable();
    break;
  default:
    ;
  }
}

uint8_t
PWMPin::duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR2B);
  case Board::PWM1: return (OCR0B);
  case Board::PWM2: return (OCR0A);
  case Board::PWM3: return (OCR1A);
  case Board::PWM4: return (OCR1B);
  case Board::PWM5: return (OCR2A);
  default:
    return (is_set());
  }
}

void
PWMPin::set(uint8_t duty)
{
  switch (m_pin) {
  case Board::PWM0:
    bit_set(TCCR2A, COM2B1);
    OCR2B = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  case Board::PWM2:
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  case Board::PWM3:
    bit_set(TCCR1A, COM1A1);
    OCR1A = duty;
    return;
  case Board::PWM4:
    bit_set(TCCR1A, COM1B1);
    OCR1B = duty;
    return;
  case Board::PWM5:
    bit_set(TCCR2A, COM2A1);
    OCR2A = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#elif defined(BOARD_ATMEGA32U4)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) :
  OutputPin((Board::DigitalPin) pin)
{
  switch (pin) {
  case Board::PWM0:
  case Board::PWM1:
    // PWM0(0A), PMW1(0B), Fast PWM, prescale 64
    TCCR0A |= _BV(WGM01) | _BV(WGM00);
    TCCR0B |= _BV(CS01) | _BV(CS00);
    break;

  case Board::PWM2:
  case Board::PWM3:
    // PWM2(1A), PWM3(1B), PWM phase correct, 10-bit, prescale 64
    TCCR1A |= _BV(WGM11) | _BV(WGM10);
    TCCR1B |= _BV(CS11) | _BV(CS10);
    break;

  case Board::PWM4:
    // PWM4(3A), PWM phase correct, 10-bit, prescale 64
    TCCR3A |= _BV(WGM31) | _BV(WGM30);
    TCCR3B |= _BV(CS31) | _BV(CS30);
    break;

  case Board::PWM5:
  case Board::PWM6:
    // PWM5(4A), PMW6(4D), Fast PWM, prescale 64
    TCCR4A |= _BV(PWM4A);
    TCCR4B |= _BV(CS42) | _BV(CS41) | _BV(CS40);
    TCCR4C |= _BV(PWM4D);
    break;
  }
  set(duty);
}

void
PWMPin::begin()
{
  switch (m_pin) {
  case Board::PWM0:
  case Board::PWM1:
    Power::timer1_enable();
    break;
  case Board::PWM2:
  case Board::PWM3:
    Power::timer0_enable();
    break;
  case Board::PWM4:
    Power::timer3_enable();
    break;
  case Board::PWM5:
  case Board::PWM6:
    // Not supported by avr/power.h
    // Power::timer4_enable();
    break;
  default:
    ;
  }
}

uint8_t
PWMPin::duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR0A);
  case Board::PWM1: return (OCR0B);
  case Board::PWM2: return (OCR1A);
  case Board::PWM3: return (OCR1B);
  case Board::PWM4: return (OCR3A);
  case Board::PWM5: return (OCR4A);
  case Board::PWM6: return (OCR4D);
  default:
    return (is_set());
  }
}

void
PWMPin::set(uint8_t duty)
{
  switch (m_pin) {
  case Board::PWM0:
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  case Board::PWM2:
    bit_set(TCCR1A, COM1A1);
    OCR1A = duty;
    return;
  case Board::PWM3:
    bit_set(TCCR1A, COM1B1);
    OCR1B = duty;
    return;
  case Board::PWM4:
    bit_set(TCCR3A, COM3A1);
    OCR3A = duty;
    return;
  case Board::PWM5:
    bit_set(TCCR4A, COM4A1);
    bit_clear(TCCR4A, COM4A0);
    OCR4A = duty;
    return;
  case Board::PWM6:
    bit_set(TCCR4C, COM4D1);
    OCR4D = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#elif defined(BOARD_ATMEGA1248P)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) :
  OutputPin((Board::DigitalPin) pin)
{
  switch (pin) {
  case Board::PWM0:
  case Board::PWM1:
    // PWM0(0A), PMW1(0A), Fast PWM, prescale 64
    TCCR0A |= _BV(WGM01) | _BV(WGM00);
    TCCR0B |= _BV(CS01) | _BV(CS00);
    break;

#if defined(__AVR_ATmega1284P__)
  case Board::PWM2:
  case Board::PWM3:
    // PWM2(3A), PWM3(3B) PWM phase correct, 8-bit, prescale 64
    TCCR3A |= _BV(WGM30);
    TCCR3B |= _BV(CS31) | _BV(CS30);
#endif
    break;

  case Board::PWM4:
  case Board::PWM5:
    // PWM5(1A), PWM4(1B), PWM phase correct, 8-bit, prescale 64
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(CS11) | _BV(CS10);
    break;

  case Board::PWM6:
  case Board::PWM7:
    // PWM6(2B), PWM7(2A), PWM phase correct, prescale 64
    TCCR2A |= _BV(WGM20);
    TCCR2B |= _BV(CS22);
    break;
  default:
    break;
  }
  set(duty);
}

void
PWMPin::begin()
{
  switch (m_pin) {
  case Board::PWM0:
  case Board::PWM1:
    Power::timer0_enable();
    break;
#if defined(__AVR_ATmega1284P__)
  case Board::PWM2:
  case Board::PWM3:
    Power::timer3_enable();
#endif
  case Board::PWM4:
  case Board::PWM5:
    Power::timer1_enable();
    break;
  case Board::PWM6:
  case Board::PWM7:
    Power::timer2_enable();
    break;
  default:
    ;
  }
}

uint8_t
PWMPin::duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR0A);
  case Board::PWM1: return (OCR0B);
#if defined(__AVR_ATmega1284P__)
  case Board::PWM2: return (OCR3A);
  case Board::PWM3: return (OCR3B);
#endif
  case Board::PWM4: return (OCR1B);
  case Board::PWM5: return (OCR1A);
  case Board::PWM6: return (OCR2B);
  case Board::PWM7: return (OCR2A);
  default:
    return (is_set());
  }
}

void
PWMPin::set(uint8_t duty)
{
  switch (m_pin) {
  case Board::PWM0:
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR0B, COM0B1);
    OCR0B = duty;
    return;
#if defined(__AVR_ATmega1284P__)
  case Board::PWM2:
    bit_set(TCCR3A, COM3A1);
    OCR3A = duty;
    return;
  case Board::PWM3:
    bit_set(TCCR3B, COM3B1);
    OCR3B = duty;
    return;
#endif
  case Board::PWM4:
    bit_set(TCCR1B, COM1B1);
    OCR1B = duty;
    return;
  case Board::PWM5:
    bit_set(TCCR1A, COM1A1);
    OCR1A = duty;
    return;
  case Board::PWM6:
    bit_set(TCCR2B, COM2B1);
    OCR2B = duty;
    return;
  case Board::PWM7:
    bit_set(TCCR2A, COM2A1);
    OCR2A = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#elif defined(BOARD_ATMEGA2560)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) :
  OutputPin((Board::DigitalPin) pin)
{
  switch (pin) {
  case Board::PWM2:
  case Board::PWM11:
    // PWM2(0B), PMW11(0A), Fast PWM, prescale 64
    TCCR0A |= _BV(WGM01) | _BV(WGM00);
    TCCR0B |= _BV(CS01) | _BV(CS00);
    break;

  case Board::PWM9:
  case Board::PWM10:
    // PWM9(1A), PWM10(1B), PWM phase correct, 8-bit, prescale 64
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(CS11) | _BV(CS10);
    break;

  case Board::PWM7:
  case Board::PWM8:
    // PWM7(2B), PWM8(2A), PWM phase correct, prescale 64
    TCCR2A |= _BV(WGM20);
    TCCR2B |= _BV(CS22);
    break;

  case Board::PWM3:
  case Board::PWM0:
  case Board::PWM1:
    // PWM3(3A), PWM0(3B), PWM1(3C) PWM phase correct, 8-bit, prescale 64
    TCCR3A |= _BV(WGM30);
    TCCR3B |= _BV(CS31) | _BV(CS30);
    break;

  case Board::PWM4:
  case Board::PWM5:
  case Board::PWM6:
    // PWM4(3A), PWM5(3B), PWM6(3C) PWM phase correct, 8-bit, prescale 64
    TCCR4A |= _BV(WGM40);
    TCCR4B |= _BV(CS41) | _BV(CS40);
    break;
  }
  set(duty);
}

void
PWMPin::begin()
{
  switch (m_pin) {
  case Board::PWM0:
  case Board::PWM1:
  case Board::PWM3:
    Power::timer3_enable();
    break;
  case Board::PWM2:
  case Board::PWM11:
    Power::timer0_enable();
    break;
  case Board::PWM4:
  case Board::PWM5:
  case Board::PWM6:
    Power::timer4_enable();
    break;
  case Board::PWM7:
  case Board::PWM8:
    Power::timer2_enable();
    break;
  case Board::PWM9:
  case Board::PWM10:
    Power::timer0_enable();
    break;
  default:
    ;
  }
}

uint8_t
PWMPin::duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR3B);
  case Board::PWM1: return (OCR3C);
  case Board::PWM2: return (OCR0B);
  case Board::PWM3: return (OCR3A);
  case Board::PWM4: return (OCR4A);
  case Board::PWM5: return (OCR4B);
  case Board::PWM6: return (OCR4C);
  case Board::PWM7: return (OCR2B);
  case Board::PWM8: return (OCR2A);
  case Board::PWM9: return (OCR1A);
  case Board::PWM10: return (OCR1B);
  case Board::PWM11: return (OCR0A);
  default:
    return (is_set());
  }
}

void
PWMPin::set(uint8_t duty)
{
  switch (m_pin) {
  case Board::PWM0:
    bit_set(TCCR3A, COM3B1);
    OCR3B = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR3A, COM3C1);
    OCR3C = duty;
    return;
  case Board::PWM2:
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  case Board::PWM3:
    bit_set(TCCR3A, COM3A1);
    OCR3A = duty;
    return;
  case Board::PWM4:
    bit_set(TCCR4A, COM4A1);
    OCR4A = duty;
    return;
  case Board::PWM5:
    bit_set(TCCR4A, COM4B1);
    OCR4B = duty;
    return;
  case Board::PWM6:
    bit_set(TCCR4A, COM4C1);
    OCR4C = duty;
    return;
  case Board::PWM7:
    bit_set(TCCR2A, COM2B1);
    OCR2B = duty;
    return;
  case Board::PWM8:
    bit_set(TCCR2A, COM2A1);
    OCR2A = duty;
    return;
  case Board::PWM9:
    bit_set(TCCR1A, COM1A1);
    OCR1A = duty;
    return;
  case Board::PWM10:
    bit_set(TCCR1A, COM1B1);
    OCR1B = duty;
    return;
  case Board::PWM11:
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#elif defined(BOARD_ATTINYX4)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) :
  OutputPin((Board::DigitalPin) pin)
{
  switch (pin) {
  case Board::PWM0:
  case Board::PWM1:
    TCCR0A |= _BV(WGM01) | _BV(WGM00);
    TCCR0B |= _BV(CS01)  | _BV(CS00);
    break;
  case Board::PWM2:
  case Board::PWM3:
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(CS11) | _BV(CS10);
    break;
  }
  set(duty);
}

void
PWMPin::begin()
{
  switch (m_pin) {
  case Board::PWM0:
  case Board::PWM1:
    Power::timer0_enable();
    break;
  case Board::PWM2:
  case Board::PWM3:
    Power::timer1_enable();
    break;
  default:
    ;
  }
}

uint8_t
PWMPin::duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR0A);
  case Board::PWM1: return (OCR0B);
  case Board::PWM2: return (OCR1A);
  case Board::PWM3: return (OCR1B);
  default:
    return (is_set());
  }
}

void
PWMPin::set(uint8_t duty)
{
  switch (m_pin) {
  case Board::PWM0:
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  case Board::PWM2:
    bit_set(TCCR1A, COM1A1);
    OCR1A = duty;
    return;
  case Board::PWM3:
    bit_set(TCCR1A, COM1B1);
    OCR1B = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#elif defined(BOARD_ATTINYX5)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) :
  OutputPin((Board::DigitalPin) pin)
{
  TCCR0A |= _BV(WGM01) | _BV(WGM00);
  TCCR0B |= _BV(CS01) | _BV(CS00);
  set(duty);
}

void
PWMPin::begin()
{
  switch (m_pin) {
  case Board::PWM0:
  case Board::PWM1:
    Power::timer0_enable();
    break;
  default:
    ;
  }
}

uint8_t
PWMPin::duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR0A);
  case Board::PWM1: return (OCR0B);
  default:
    return (is_set());
  }
}

void
PWMPin::set(uint8_t duty)
{
  switch (m_pin) {
  case Board::PWM0:
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#elif defined(BOARD_ATTINYX61)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) :
  OutputPin((Board::DigitalPin) pin)
{
  // Prescale(64)
  TCCR1B |= _BV(CS12)  | _BV(CS11)  | _BV(CS10);

  // PWM mode
  switch (m_pin) {
  case Board::PWM0:
    TCCR1A |= _BV(PWM1A);
    break;
  case Board::PWM1:
    TCCR1A |= _BV(PWM1B);
    break;
  case Board::PWM2:
    TCCR1C |= _BV(PWM1D);
    break;
  }
  set(duty);
}

void
PWMPin::begin()
{
  switch (m_pin) {
  case Board::PWM0:
  case Board::PWM1:
  case Board::PWM2:
    Power::timer1_enable();
    break;
  default:
    ;
  }
}

uint8_t
PWMPin::duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR1A);
  case Board::PWM1: return (OCR1B);
  case Board::PWM2: return (OCR1D);
  default:
    return (is_set());
  }
}

void
PWMPin::set(uint8_t duty)
{
  switch (m_pin) {
  case Board::PWM0:
    bit_set(TCCR1C, COM1A1);
    OCR1A = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR1C, COM1B1);
    OCR1B = duty;
    return;
  case Board::PWM2:
    bit_set(TCCR1C, COM1D1);
    OCR1D = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#endif

void
PWMPin::set(uint16_t value, uint16_t min, uint16_t max)
{
  uint8_t duty;
  if (UNLIKELY(value <= min)) duty = 0;
  else if (UNLIKELY(value >= max)) duty = 255;
  else duty = (((uint32_t) (value - min)) << 8) / (max - min);
  set(duty);
}

