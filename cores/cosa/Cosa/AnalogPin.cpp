/**
 * @file Cosa/AnalogPin.cpp
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

bool
AnalogPin::sample_request(Board::AnalogPin pin, uint8_t ref)
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  if (UNLIKELY(sampling_pin != NULL)) return (false);
  sampling_pin = this;
  ADMUX = (ref | (pin & 0x1f));
#if defined(MUX5)
  bit_write(pin & 0x20, ADCSRB, MUX5);
#endif
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC) | _BV(ADIE));
  return (true);
}

uint16_t
AnalogPin::sample_await()
{
  if (UNLIKELY(sampling_pin != this)) return (m_value);
  synchronized {
    sampling_pin = NULL;
    bit_clear(ADCSRA, ADIE);
  }
  loop_until_bit_is_clear(ADCSRA, ADSC);
  synchronized m_value = ADCW;
  return (m_value);
}

void
AnalogPin::on_event(uint8_t type, uint16_t value)
{
  if (type == Event::TIMEOUT_TYPE) {
    sample_request(m_event);
  }
  else if (type == Event::SAMPLE_COMPLETED_TYPE) {
    if (value != m_value) {
      m_value = value;
      on_change(value);
    }
  }
}

void
AnalogPin::on_interrupt(uint16_t value)
{
  uint8_t event = sampling_pin->m_event;
  if (event == Event::NULL_TYPE)
    sampling_pin->m_value = value;
  else
    Event::push(event, this, value);
  sampling_pin = NULL;
}

ISR(ADC_vect)
{
  bit_clear(ADCSRA, ADIE);
  if (UNLIKELY(AnalogPin::sampling_pin == NULL)) return;
  AnalogPin::sampling_pin->on_interrupt(ADCW);
}
