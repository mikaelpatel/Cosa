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

AnalogPin* AnalogPin::sampling_pin = NULL;

void 
AnalogPin::prescale(uint8_t factor)
{
  const uint8_t MASK = (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
  bit_field_set(ADCSRA, MASK, factor);
}

bool
AnalogPin::sample_request(Board::AnalogPin pin, uint8_t ref)
{
  if (sampling_pin != NULL) return (false);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  sampling_pin = this;
  ADMUX = (ref | (pin & 0x1f));
#if defined(MUX5)
  bit_write(pin & 0x20, ADCSRB, MUX5);
#endif
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC) | _BV(ADIE));
  return (true);
}

uint16_t 
AnalogPin::bandgap(uint16_t vref)
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (Board::AVCC_REFERENCE | Board::VBG);
#if defined(MUX5)
  bit_clear(ADCSRB, MUX5);
#endif
  bit_set(ADCSRA, ADEN);
  delay(1);
  bit_set(ADCSRA, ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  uint16_t sample;
  synchronized {
    sample = ADCW;
  }
  return ((vref * 1024L) / sample);
}

uint16_t 
AnalogPin::sample(Board::AnalogPin pin, Board::Reference ref)
{
  if (sampling_pin != NULL) return (0xffffU);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (ref | (pin & 0x1f));
#if defined(MUX5)
  bit_write(pin & 0x20, ADCSRB, MUX5);
#endif
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (ADCW);
}

uint16_t 
AnalogPin::sample_await()
{
  if (sampling_pin != this) return (m_value);
  synchronized {
    sampling_pin = NULL;
    bit_clear(ADCSRA, ADIE);
  }
  loop_until_bit_is_clear(ADCSRA, ADSC);
  synchronized {
    m_value = ADCW;
  }
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
  if (AnalogPin::sampling_pin == NULL) return;
  AnalogPin::sampling_pin->on_interrupt(ADCW);
}

