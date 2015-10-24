/**
 * @file Cosa/RTT_Config.hh
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

#ifndef COSA_RTT_CONFIG_HH
#define COSA_RTT_CONFIG_HH

// Real-Time Timer Configuration
#define COUNT 250
#define PRESCALE 64
#define TIMER_MAX (COUNT - 1)
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK (COUNT * US_PER_TIMER_CYCLE)
#define MS_PER_TICK (US_PER_TICK / 1000)
#define US_DIRECT_EXPIRE (800 / I_CPU)
#define US_TIMER_EXPIRE (US_PER_TICK - 1)

// Real-Time Timer Registers. Use Timer2 if available to keep
// timer running in low power mode
#if defined(TIMER2_COMPA_vect)
#define timern_enable timer2_enable
#define timern_disable timer2_disable
#define TCCRnB TCCR2B
#define CSn _BV(CS22)
#define TCCRnA TCCR2A
#define OCRnA OCR2A
#define OCRnB OCR2B
#define TIMSKn TIMSK2
#define TCNTn TCNT2
#define TIFRn TIFR2
#define TIMERn_COMPA_vect TIMER2_COMPA_vect
#define TIMERn_COMPB_vect TIMER2_COMPB_vect
#elif defined(TCNT0)
#define timern_enable timer0_enable
#define timern_disable timer0_disable
#define TCCRnB TCCR0B
#define CSn (_BV(CS01) | _BV(CS00))
#define TCCRnA TCCR0A
#define OCRnA OCR0A
#define OCRnB OCR0B
#define TIMSKn TIMSK0
#define TCNTn TCNT0
#define TIFRn TIFR0
#define TIMERn_COMPA_vect TIMER0_COMPA_vect
#define TIMERn_COMPB_vect TIMER0_COMPB_vect
#endif
#endif
