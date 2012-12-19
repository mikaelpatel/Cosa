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

#include "Cosa/Pins.h"
#include <util/delay_basic.h>

#define DELAY(us) _delay_loop_2((us) << 2)

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
		  _pin, _sfr, _mask);
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
  if (_pin == 5) { 
    bit_set(TCCR0A, COM0B1);
    OCR0B = duty;
    return;
  }
  if (_pin == 6) {
    bit_set(TCCR0A, COM0A1);
    OCR0A = duty;
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
  if (_pin == 5) return (OCR0B);
  if (_pin == 6) return (OCR0A);
  return (is_set());
}

AnalogPin* AnalogPin::sampling_pin = 0;

uint16_t 
AnalogPin::sample()
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (_reference | ((_pin - 14) & 0xf));
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (_value = ADCW);
}

void 
AnalogPin::sample_request()
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (_reference | (_pin - 14));
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  if (_handler == 0) return;
  sampling_pin = this;
  bit_set(ADCSRA, ADIE);
}

uint16_t 
AnalogPin::sample_await()
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (_value = ADCW);
}

ISR(ADC_vect) 
{ 
  bit_clear(ADCSRA, ADIE);
  AnalogPin* pin = AnalogPin::get_sampling_pin();
  if (pin != 0) pin->on_sample(ADCW);
}

void 
AnalogPins::sample_next(AnalogPin* pin, void* env)
{
  AnalogPins* set = (AnalogPins*) env;
  if (set->_next != set->_count) {
    AnalogPin* pin = set->get_pin_at(set->_next++);
    pin->sample_request();
  } 
  else if (set->_handler != 0) {
    set->_handler(set, set->_env);
  }
}

bool
AnalogPins::samples_request(InterruptHandler fn, void* env)
{
  if (AnalogPin::get_sampling_pin() != 0) return (0);
  if (fn != 0) {
    _handler = fn;
    _env = env;
  }
  get_pin_at(0)->sample_request();
  _next = 1;
  return (1);
}

