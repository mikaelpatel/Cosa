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
  if (InterruptPin::ext[0] != 0) InterruptPin::ext[0]->on_interrupt();
}

ISR(INT1_vect)
{
  if (InterruptPin::ext[1] != 0) InterruptPin::ext[1]->on_interrupt();
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
  switch (m_pin) {
  case 3:
    bit_set(TCCR2A, COM2B1);
    OCR2B = duty;
    return;
  case 5:
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  case 6:
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
    return;
  case 11:
    bit_set(TCCR2A, COM2A1);
    OCR2A = duty;
    return;
  default:
    OutputPin::set(duty);
  }
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
  switch (m_pin) {
  case 3: return (OCR2B);
  case 5: return (OCR0B);
  case 6: return (OCR0A);
  case 11: return (OCR2A);
  default:
    return (is_set());
  }
}

AnalogPin* AnalogPin::sampling_pin = 0;

bool
AnalogPin::sample_request(uint8_t pin, uint8_t ref)
{
  if (sampling_pin != 0) return (0);
  if (pin >= 14) pin -= 14;
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (ref | pin);
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  sampling_pin = this;
  bit_set(ADCSRA, ADIE);
  return (1);
}

uint16_t 
AnalogPin::sample(uint8_t pin, Reference ref)
{
  if (sampling_pin != 0) return (0);
  if (pin >= 14) pin -= 14;
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (ref | pin);
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (ADCW);
}

uint16_t 
AnalogPin::sample()
{
  return (m_value = AnalogPin::sample(m_pin, (Reference) m_reference));
}

bool
AnalogPin::sample_request()
{
  return (sample_request(m_pin, (Reference) m_reference));
}

uint16_t 
AnalogPin::sample_await()
{
  if (sampling_pin == 0) return (0xffffU);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (m_value = ADCW);
}

void 
AnalogPin::on_interrupt(uint16_t value)
{ 
  sampling_pin = 0;
  Event::push(Event::SAMPLE_COMPLETED_TYPE, this, value);
}

void 
AnalogPin::on_event(uint8_t type, uint16_t value)
{
  // On timeout events, pins have been attached, issue a sample request
  if (type == Event::TIMEOUT_TYPE) {
    sample_request();
  }
  // When the sample request is completed check for change and call action
  else if (type == Event::SAMPLE_COMPLETED_TYPE) {
    if (value != m_value) {
      m_value = value;
      on_change(value);
    }
  }
}

ISR(ADC_vect) 
{ 
  bit_clear(ADCSRA, ADIE);
  if (AnalogPin::sampling_pin != 0) 
    AnalogPin::sampling_pin->on_interrupt(ADCW);
}

bool
AnalogPins::samples_request()
{
  m_next = 0;
  return (AnalogPin::sample_request(get_pin_at(m_next), m_reference));
}

void 
AnalogPins::on_interrupt(uint16_t value)
{
  sampling_pin = 0;
  m_buffer[m_next++] = value;
  if (m_next != m_count) {
    AnalogPin::sample_request(get_pin_at(m_next), m_reference);
  } 
  else {
    Event::push(Event::SAMPLE_COMPLETED_TYPE, this, value);
  }
}

