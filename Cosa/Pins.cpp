/**
 * @file Cosa/Pins.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"

uint8_t 
Pin::read(OutputPin& clk, Direction order)
{
  uint8_t value = 0;
  uint8_t bits = CHARBITS;
  if (order == MSB_FIRST) {
    do {
      clk.set();
      value <<= 1;
      if (is_set()) value |= 0x01;
      clk.clear();
    } while (--bits);
  }
  else {
    do {
      clk.set();
      value >>= 1;
      if (is_set()) value |= 0x80;
      clk.clear();
    } while (--bits);
  }
  return (value);
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
  print(stream);
  stream.println();
}

void 
OutputPin::write(uint8_t value, OutputPin& clk, Direction order)
{
  uint8_t bits = CHARBITS;
  if (order == MSB_FIRST) {
    do {
      write(value & 0x80);
      value <<= 1;
      clk.set();
      DELAY(1);
      clk.clear();
    } while (--bits);
  }
  else {
    do {
      write(value & 0x01);
      value >>= 1;
      clk.set();
      DELAY(1);
      clk.clear();
    } while (--bits);
  }
}

void 
OutputPin::pulse(uint16_t us)
{
  toggle();
  DELAY(us);
  toggle();
}

void 
OutputPin::pulse(uint8_t value, uint16_t us)
{
  uint8_t bits = CHARBITS;
  synchronized {
    write(0);
    DELAY(us);
    do {
      write(value & 0x01);
      DELAY(us);
      value >>= 1;
    } while (--bits);
    write(1);
  }
  DELAY(us);
}

#if defined(__ARDUINO_STANDARD__)

uint8_t
PWMPin::get_duty()
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
    bit_set(TCCR1B, COM1B1);
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

#elif defined(__ARDUINO_MIGHTY__)

uint8_t
PWMPin::get_duty()
{
  switch (m_pin) {
  case Board::PWM0: return (OCR0A);
  case Board::PWM1: return (OCR0B);
  case Board::PWM2: return (OCR3A);
  case Board::PWM3: return (OCR3B);
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
  case Board::PWM2:
    bit_set(TCCR3A, COM3A1);
    OCR3A = duty;
    return;
  case Board::PWM3:
    bit_set(TCCR3B, COM3B1);
    OCR3B = duty;
    return;
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

#elif defined(__ARDUINO_MEGA__)

uint8_t
PWMPin::get_duty()
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
    bit_set(TCCR3B, COM3B1);
    OCR3B = duty;
    return;
  case Board::PWM1:
    bit_set(TCCR3C, COM3C1);
    OCR3C = duty;
    return;
  case Board::PWM2:
    bit_set(TCCR0B, COM0B1);
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
    bit_set(TCCR4B, COM4B1);
    OCR4B = duty;
    return;
  case Board::PWM6:
    bit_set(TCCR4C, COM4C1);
    OCR4C = duty;
    return;
  case Board::PWM7:
    bit_set(TCCR2B, COM2B1);
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
    bit_set(TCCR1B, COM1B1);
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

#elif defined(__ARDUINO_TINYX5__)

uint8_t
PWMPin::get_duty()
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
    bit_set(TCCR0B, COM0B1);
    OCR0B = duty;
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
  if (value <= min) duty = 0;
  else if (value >= max) duty = 255;
  else duty = (((uint32_t) (value - min)) << 8) / (max - min);
  set(duty);
}

AnalogPin* AnalogPin::sampling_pin = 0;

void 
AnalogPin::prescale(uint8_t factor)
{
  const uint8_t MASK = (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
  bit_field_set(ADCSRA, MASK, factor);
}

bool
AnalogPin::sample_request(uint8_t pin, uint8_t ref)
{
  if (sampling_pin != 0) return (false);
  if (pin >= Board::A0) pin -= Board::A0;
  loop_until_bit_is_clear(ADCSRA, ADSC);
  sampling_pin = this;
  ADMUX = (ref | pin);
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC) | _BV(ADIE));
  return (true);
}

uint16_t 
AnalogPin::bandgap(uint16_t vref)
{
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (AnalogPin::AVCC_REFERENCE | Board::VBG);
  bit_mask_set(ADCSRA, _BV(ADEN));
#if defined(__ARDUINO_TINYX5__)
  DELAY(1000);
#endif
  bit_mask_set(ADCSRA, _BV(ADSC));
  loop_until_bit_is_clear(ADCSRA, ADSC);
  uint16_t sample = ADCW;
  return ((vref * 1024L) / sample);
}

uint16_t 
AnalogPin::sample(uint8_t pin, Reference ref)
{
  if (sampling_pin != 0) return (0xffffU);
  if (pin >= Board::A0) pin -= Board::A0;
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (ref | pin);
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (ADCW);
}

uint16_t 
AnalogPin::sample_await()
{
  if (sampling_pin != this) return (m_value);
  synchronized {
    sampling_pin = 0;
    bit_clear(ADCSRA, ADIE);
  }
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (m_value = ADCW);
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
  sampling_pin->m_value = value;
  uint8_t event = sampling_pin->m_event;
  if (event != Event::NULL_TYPE) Event::push(event, this, value);
  sampling_pin = 0;
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

AnalogComparator* AnalogComparator::comparator = 0;

void 
AnalogComparator::on_interrupt(uint16_t arg) 
{ 
  Event::push(Event::CHANGE_TYPE, this, arg);
}

ISR(ANALOG_COMP_vect)
{ 
  if (AnalogComparator::comparator != 0) 
    AnalogComparator::comparator->on_interrupt();
}
