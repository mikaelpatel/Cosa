/**
 * @file Cosa/AnalogPin_static.cpp
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

#include "Cosa/AnalogPin.hh"

AnalogPin* AnalogPin::sampling_pin = NULL;

void
AnalogPin::prescale(uint8_t factor)
{
  const uint8_t MASK = (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
  bit_field_set(ADCSRA, MASK, factor);
}

uint16_t
AnalogPin::bandgap(uint16_t vref)
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  if (UNLIKELY(sampling_pin != NULL)) return (UINT16_MAX);
  ADMUX = (Board::AVCC_REFERENCE | Board::VBG);
#if defined(MUX5)
  bit_clear(ADCSRB, MUX5);
#endif
  DELAY(500);
  bit_set(ADCSRA, ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  uint16_t sample;
  synchronized sample = ADCW;
  return ((vref * 1024L) / sample);
}

uint16_t
AnalogPin::sample(Board::AnalogPin pin, Board::Reference ref)
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  if (UNLIKELY(sampling_pin != NULL)) return (UINT16_MAX);
  ADMUX = (ref | (pin & 0x1f));
#if defined(MUX5)
  bit_write(pin & 0x20, ADCSRB, MUX5);
#endif
  bit_set(ADCSRA, ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (ADCW);
}
