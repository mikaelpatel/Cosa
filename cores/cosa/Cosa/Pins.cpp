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

IOStream& operator<<(IOStream& outs, Pin& pin)
{
  outs << PSTR("Pin(pin = ") << (uint8_t) pin.m_pin 
       << PSTR(", sfr = ") << (void*) pin.m_sfr 
       << PSTR(", mask = ") << bin << (uint8_t) pin.m_mask << ')';
  return (outs);
}

void 
OutputPin::write(uint8_t value, OutputPin& clk, Direction order)
{
  uint8_t bits = CHARBITS;
  if (order == MSB_FIRST) {
    synchronized do {
      _write(value & 0x80);
      clk._toggle();
      value <<= 1;
      clk._toggle();
    } while (--bits);
  }
  else {
    synchronized do {
      _write(value & 0x01);
      clk._toggle();
      value >>= 1;
      clk._toggle();
    } while (--bits);
  }
}

void 
OutputPin::write(uint16_t value, uint8_t bits, uint16_t us)
{
  if (bits == 0) return;
  synchronized {
    do {
      _write(value & 0x01);
      DELAY(us);
      value >>= 1;
    } while (--bits);
  }
}

#if defined(__ARDUINO_STANDARD__)

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

#elif defined(__ARDUINO_MIGHTY__)

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

  case Board::PWM2:
  case Board::PWM3:
    // PWM2(3A), PWM3(3B) PWM phase correct, 8-bit, prescale 64
    TCCR3A |= _BV(WGM30);
    TCCR3B |= _BV(CS31) | _BV(CS30);
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
  }
  set(duty); 
}

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

#elif defined(__ARDUINO_TINYX4__)

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

uint8_t
PWMPin::get_duty()
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
    bit_set(TCCR0B, COM0B1);
    OCR0B = duty;
    return;
  case Board::PWM2:
    bit_set(TCCR1A, COM1A1);
    OCR0A = duty;
    return;
  case Board::PWM3:
    bit_set(TCCR1B, COM1B1);
    OCR0B = duty;
    return;
  default:
    OutputPin::set(duty);
  }
}

#elif defined(__ARDUINO_TINYX5__)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) : 
  OutputPin((Board::DigitalPin) pin) 
{ 
  TCCR0A |= _BV(WGM01) | _BV(WGM00);
  TCCR0B |= _BV(CS01)  | _BV(CS00);
  set(duty); 
}

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

#elif defined(__ARDUINO_TINYX61__)

PWMPin::PWMPin(Board::PWMPin pin, uint8_t duty) : 
  OutputPin((Board::DigitalPin) pin) 
{ 
  TCCR1B |= _BV(CS11)  | _BV(CS10);
  TCCR1D |= _BV(WGM11) | _BV(WGM10);
  set(duty); 
}

uint8_t
PWMPin::get_duty()
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
  if (value <= min) duty = 0;
  else if (value >= max) duty = 255;
  else duty = (((uint32_t) (value - min)) << 8) / (max - min);
  set(duty);
}

AnalogPin* AnalogPin::sampling_pin = NULL;

void 
AnalogPin::prescale(uint8_t factor)
{
  const uint8_t MASK = (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
  bit_field_set(ADCSRA, MASK, factor);
}

bool
AnalogPin::sample_request(uint8_t pin, uint8_t ref)
{
  if (sampling_pin != NULL) return (false);
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
  ADMUX = (AVCC_REFERENCE | Board::VBG);
  bit_set(ADCSRA, ADEN);
  DELAY(1000);
  bit_set(ADCSRA, ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  uint16_t sample = ADCW;
  return ((vref * 1024L) / sample);
}

uint16_t 
AnalogPin::sample(uint8_t pin, Reference ref)
{
  if (sampling_pin != NULL) return (0xffffU);
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
    sampling_pin = NULL;
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
  if (AnalogPin::sampling_pin != NULL) 
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

AnalogComparator* AnalogComparator::comparator = NULL;

void 
AnalogComparator::on_interrupt(uint16_t arg) 
{ 
  Event::push(Event::CHANGE_TYPE, this, arg);
}

ISR(ANALOG_COMP_vect)
{ 
  if (AnalogComparator::comparator != NULL) 
    AnalogComparator::comparator->on_interrupt();
}
