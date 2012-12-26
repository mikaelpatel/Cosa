/**
 * @file Cosa/Pins.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Arduino pins abstractions; abstract, input, output, interrupt and 
 * analog pin. Captures the mapping from Arduino to processor pins.
 * Forces declarative programming of pins in sketches.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"

uint8_t 
Pin::await_change(uint8_t us)
{
  uint8_t res = 0;
  synchronized {
    if (is_set()) {
      while (is_set() && us--) res++;
    }
    else {
      while (is_clear() && us--) res++;
    }
  }
  return (res);
}

void 
Pin::print(IOStream& stream)
{
  stream.printf_P(PSTR("Pin(pin = %d, sfr = %p, mask = %bd)"), 
		  m_pin, m_sfr, m_mask);
}

void 
Pin::println(IOStream& stream)
{
  print();
  stream.println();
}

InterruptPin* InterruptPin::ext[2] = { 0, 0 };

ISR(INT0_vect)
{
  if (InterruptPin::ext[0] != 0) 
    InterruptPin::ext[0]->on_interrupt();
}

ISR(INT1_vect)
{
  if (InterruptPin::ext[1] != 0) 
    InterruptPin::ext[1]->on_interrupt();
}

void 
OutputPin::pulse(uint16_t us)
{
  toggle();
  DELAY(us);
  toggle();
}

void 
PWMPin::set(uint8_t duty)
{
  if (m_pin == 3) { 
    bit_set(TCCR2A, COM2B1);
    OCR2B = duty;
    return;
  }
  if (m_pin == 5) { 
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  }
  if (m_pin == 6) {
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  }
  if (m_pin == 11) { 
    bit_set(TCCR2A, COM2A1);
    OCR2A = duty;
    return;
  }
  if (duty < 128) 
    OutputPin::clear(); 
  else 
    OutputPin::set();
}

void 
PWMPin::set(uint16_t value, uint16_t min, uint16_t max)
{
  uint8_t duty;
  if (value <= min) duty = 0;
  else if (value >= max) duty = 255;
  else duty = (((uint32_t) (value - min)) << 8) / (max - min);
  set(duty);
}

uint8_t
PWMPin::get_duty()
{
  if (m_pin == 3) return (OCR2B);
  if (m_pin == 5) return (OCR0B);
  if (m_pin == 6) return (OCR0A);
  if (m_pin == 11) return (OCR2A);
  return (is_set());
}

AnalogPin* AnalogPin::sampling_pin = 0;

uint16_t 
AnalogPin::sample()
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (m_reference | ((m_pin - 14) & 0xf));
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (m_value = ADCW);
}

bool
AnalogPin::sample_request()
{
  if (sampling_pin != 0) return (0);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (m_reference | (m_pin - 14));
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  sampling_pin = this;
  bit_set(ADCSRA, ADIE);
  return (1);
}

uint16_t 
AnalogPin::sample_await()
{
  if (sampling_pin == 0) return (0);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (m_value = ADCW);
}

ISR(ADC_vect) 
{ 
  bit_clear(ADCSRA, ADIE);
  AnalogPin* pin = AnalogPin::get_sampling_pin();
  if (pin != 0) pin->on_interrupt(ADCW);
}

bool
AnalogPins::samples_request()
{
  if (AnalogPin::get_sampling_pin() != 0) return (0);
  uint8_t pin = get_pin_at(0);
  if (pin >= 14) pin -= 14;
  m_next = 0;
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (m_reference | pin);
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  sampling_pin = this;
  bit_set(ADCSRA, ADIE);
  return (1);
}

void 
AnalogPins::on_interrupt(uint16_t value)
{
  m_buffer[m_next++] = value;
  if (m_next != m_count) {
    uint8_t pin = get_pin_at(m_next);
    if (pin >= 14) pin -= 14;
    loop_until_bit_is_clear(ADCSRA, ADSC);
    ADMUX = (m_reference | pin);
    bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
    sampling_pin = this;
    bit_set(ADCSRA, ADIE);
  } 
  else {
    sampling_pin = 0;
    Event::push(Event::SAMPLE_COMPLETED_TYPE, this);
  }
}

