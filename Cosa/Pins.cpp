/**
 * @file Cosa/Pins.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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

#include "Pins.h"
#include <util/delay_basic.h>

void 
Pin::print()
{
#ifndef NDEBUG
  Serial_print("Pin(");
  Serial.print(_pin);
  Serial_print(", 0x");
  Serial.print((uint16_t) _sfr, HEX);
  Serial_print(", 0b");
  Serial.print(_mask, BIN);
  Serial_print(")");
#endif
}

void 
Pin::println() 
{
#ifndef NDEBUG
  print();
  Serial_print("\n");
#endif
}

InputPin::InputPin(uint8_t pin, Mode mode) : 
  Pin(pin)
{
  if (mode == PULLUP_MODE) *PORT() |= _mask; 
}

InterruptPin* InterruptPin::ext[2] = { 0, 0 };

InterruptPin::InterruptPin(uint8_t pin, Mode mode, Callback fn, void* env);

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
  _delay_loop_2(us >> 2);
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

AnalogPin* AnalogPin::response = 0;

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
AnalogPin::request_sample()
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (_reference | (_pin - 14));
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  if (_callback == 0) return;
  response = this;
  bit_set(ADCSRA, ADIE);
}

uint16_t 
AnalogPin::await_sample()
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (_value = ADCW);
}

ISR(ADC_vect) 
{ 
  if (AnalogPin::response != 0) {
    AnalogPin::response->on_sample(ADCW);
    AnalogPin::response = 0;
  }
  bit_clear(ADCSRA, ADIE);
}
