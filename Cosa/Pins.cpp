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
 * @section Description
 * Arduino pins abstractions; abstract, input, output, interrupt and 
 * analog pin. Captures the mapping from Arduino to processor pins.
 * Forces declarative programming of pins in sketches.
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
  print();
  stream.println();
}

#if defined(__ARDUINO_STANDARD__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  m_ix = pin - Board::EXT0;
  ext[m_ix] = this;
  uint8_t ix = (m_ix << 1);
  EICRA = (EICRA & ~(0b11 << ix)) | (mode << ix);
}

#elif defined(__ARDUINO_MEGA__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  if (pin <= Board::EXT3) {
    m_ix = Board::EXT0 - pin;
    uint8_t ix = (m_ix << 1);
    EICRA = (EICRA & ~(0b11 << ix)) | (mode << ix);
  } 
  else {
    m_ix = pin - Board::EXT4;
    uint8_t ix = (m_ix << 1);
    EICRB = (EICRB & ~(0b11 << ix)) | (mode << ix);
    m_ix += 4;
  }
  ext[m_ix] = this;
}

#elif defined(__ARDUINO_MIGHTY__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  if (pin == Board::EXT2) {
    m_ix = 2;
  } else {
    m_ix = pin - Board::EXT0;
  } 
  uint8_t ix = (m_ix << 1);
  EICRA = (EICRA & ~(0b11 << ix)) | (mode << ix);
  ext[m_ix] = this;
}

#elif defined(__ARDUINO_TINYX5__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  m_ix = 0;
  ext[m_ix] = this;
  MCUCR = (MCUCR & ~(0b11)) | (mode);
}

#endif

ExternalInterruptPin* ExternalInterruptPin::ext[Board::EXT_MAX] = { 0 };

void 
ExternalInterruptPin::on_interrupt(uint16_t arg) 
{ 
  Event::push(Event::CHANGE_TYPE, this, arg);
}

ISR(INT0_vect)
{
  if (ExternalInterruptPin::ext[0] != 0) 
    ExternalInterruptPin::ext[0]->on_interrupt();
}

#if !defined(__ARDUINO_TINYX5__)

ISR(INT1_vect)
{
  if (ExternalInterruptPin::ext[1] != 0) 
    ExternalInterruptPin::ext[1]->on_interrupt();
}

#if !defined(__ARDUINO_STANDARD__)

ISR(INT2_vect)
{
  if (ExternalInterruptPin::ext[2] != 0) 
    ExternalInterruptPin::ext[2]->on_interrupt();
}

#if defined(__ARDUINO_MEGA__)

ISR(INT3_vect)
{
  if (ExternalInterruptPin::ext[3] != 0) 
    ExternalInterruptPin::ext[3]->on_interrupt();
}

ISR(INT4_vect)
{
  if (ExternalInterruptPin::ext[4] != 0) 
    ExternalInterruptPin::ext[4]->on_interrupt();
}

ISR(INT5_vect)
{
  if (ExternalInterruptPin::ext[5] != 0) 
    ExternalInterruptPin::ext[5]->on_interrupt();
}
#endif
#endif
#endif

InterruptPin* InterruptPin::pin[Board::PIN_MAX] = { 0 };
uint8_t InterruptPin::state[Board::PCINT_MAX] = { 0 };

void 
InterruptPin::begin()
{
#if defined(__ARDUINO_MEGA__)
  state[0] = *Pin::PIN(16);
  state[1] = 0;
  state[2] = *Pin::PIN(64);
#else
  for (uint8_t i = 0; i < Board::PCINT_MAX; i++)
    state[i] = *Pin::PIN(i << 3);
#endif
  synchronized {
#if defined(__ARDUINO_TINYX5__)
    GIMSK |= PCIE;
#elif defined(__ARDUINO_MIGHTY__)
    PCICR |= (_BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#else
    PCICR |= (_BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#endif
  }
}

void 
InterruptPin::end()
{
  synchronized {
#if defined(__ARDUINO_TINYX5__)
    GIMSK &= ~PCIE;
#elif defined(__ARDUINO_MIGHTY__)
    PCICR &= ~(_BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#else
    PCICR &= ~(_BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#endif
  }
}

void
InterruptPin::on_interrupt(uint16_t arg)
{ 
  Event::push(Event::CHANGE_TYPE, this, arg);
}

#if defined(__ARDUINO_TINYX5__)

ISR(PCINT0_vect)
{
  uint8_t mask = PCMSK0;
  uint8_t state = *Pin::PIN(0);
  uint8_t changed = (state ^ InterruptPin::state[0]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i] != 0)) {
      InterruptPin::pin[i]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[0] = state;
}

#elif defined(__ARDUINO_STANDARD__)

void
InterruptPin::on_interrupt(uint8_t ix, uint8_t mask)
{
  uint8_t px = (ix << 3) - (ix < 2 ? 0 : 2);
  uint8_t state = *Pin::PIN(px);
  uint8_t changed = (state ^ InterruptPin::state[ix]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i + px] != 0)) {
      InterruptPin::pin[i + px]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[ix] = state;
}

ISR(PCINT0_vect)
{
  InterruptPin::on_interrupt(1, PCMSK0);
}

ISR(PCINT1_vect)
{
  InterruptPin::on_interrupt(2, PCMSK1);
}

ISR(PCINT2_vect)
{
  InterruptPin::on_interrupt(0, PCMSK2);
}

#elif defined(__ARDUINO_MEGA__)

void
InterruptPin::on_interrupt(uint8_t ix, uint8_t mask)
{
  uint8_t px = (ix << 3);
  uint8_t rx = (ix == 0 ? 16 : 64);
  uint8_t state = *Pin::PIN(rx);
  uint8_t changed = (state ^ InterruptPin::state[ix]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i + px] != 0)) {
      InterruptPin::pin[i + px]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[ix] = state;
}

ISR(PCINT0_vect)
{
  InterruptPin::on_interrupt(0, PCMSK0);
}

ISR(PCINT1_vect)
{
  InterruptPin::on_interrupt(1, PCMSK1);
}

ISR(PCINT2_vect)
{
  InterruptPin::on_interrupt(2, PCMSK2);
}

#elif defined(__ARDUINO_MIGHTY__)

// Fix: Not yet implemented 

void
InterruptPin::on_interrupt(uint8_t ix, uint8_t mask)
{
}

ISR(PCINT0_vect)
{
}

ISR(PCINT1_vect)
{
}

ISR(PCINT2_vect)
{
}

ISR(PCINT3_vect)
{
}
#endif

void 
OutputPin::pulse(uint16_t us)
{
  toggle();
  DELAY(us);
  toggle();
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
      clk.clear();
    } while (--bits);
  }
  else {
    do {
      write(value & 0x01);
      value >>= 1;
      clk.set();
      clk.clear();
    } while (--bits);
  }
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

bool
AnalogPin::sample_request(uint8_t pin, uint8_t ref)
{
  if (sampling_pin != 0) return (false);
  if (pin >= Board::A0) pin -= Board::A0;
  loop_until_bit_is_clear(ADCSRA, ADSC);
  ADMUX = (ref | pin);
  bit_mask_set(ADCSRA, _BV(ADEN) | _BV(ADSC));
  sampling_pin = this;
  bit_set(ADCSRA, ADIE);
  return (true);
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
