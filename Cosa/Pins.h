/**
 * @file Cosa/Pins.h
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

#ifndef __COSA_PINS_H__
#define __COSA_PINS_H__

#include "Types.h"
#include "Bits.h"
#include "Event.h"

class Pin {

protected:
  volatile uint8_t* const _sfr;
  const uint8_t _mask;
  const uint8_t _pin;

  /**
   * Return pointer to PIN register.
   * @return PIN register pointer.
   */
  volatile uint8_t* PIN() { return (_sfr); }

  /**
   * Return pointer to data direction register.
   * @return DDR register pointer.
   */
  volatile uint8_t* DDR() { return (_sfr + 1); }

  /**
   * Return pointer to data port register.
   * @return PORT register pointer.
   */
  volatile uint8_t* PORT() { return (_sfr + 2); }

  /**
   * Return special function register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  volatile uint8_t* SFR(uint8_t pin) 
  { 
    return (pin < 8 ? &PIND : (pin < 14 ? &PINB : &PINC));
  }

  /**
   * Return bit position for given Arduino pin number.
   * @param[in] pin number.
   * @return pin bit position.
   */
  const uint8_t BIT(uint8_t pin)
  {
    return (pin < 8 ? pin : (pin < 14 ? pin - 8 : (pin - 14)));
  }
  
  /**
   * Return bit mask position for given Arduino pin number.
   * @param[in] pin number.
   * @return pin bit mask.
   */
  const uint8_t MASK(uint8_t pin)
  {
    return (_BV(BIT(pin)));
  }

public:
  /**
   * Construct abstract pin given Arduino pin number.
   * @param[in] pin number.
   */
  Pin(uint8_t pin) : _sfr(SFR(pin)), _mask(MASK(pin)), _pin(pin) {}

  /**
   * Return Arduino pin number of abstract pin.
   * @return pin number.
   */
  uint8_t pin() { return (_pin); }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  uint8_t is_set() { return ((*PIN() & _mask) != 0); }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  uint8_t is_clear() { return ((*PIN() & _mask) == 0); }

  /**
   * In debug mode, print abstract pin information to serial stream.
   */
  void print();

  /**
   * In debug mode, print abstract pin information to serial stream
   * with new-line.
   */
  void println();
};

/**
 * Abstract input pin. Allows pullup mode.
 */
class InputPin : public Pin {

public:
  enum Mode {
    NORMAL_MODE = 0,
    PULLUP_MODE = 1
  };

  /**
   * Construct abstract input pin given Arduino pin number.
   * @param[in] pin number.
   * @param[in] mode pin mode (normal or pullup).
   */
  InputPin(uint8_t pin, Mode mode = NORMAL_MODE);
};

/**
 * Abstract interrupt pin. Allows callback with the pin value changes.
 */
class InterruptPin : public InputPin {

public:
  /**
   * Callback function prototype.
   * @param[in] pin reference of changed pin.
   * @param[in] env callback environment. 
   */
  typedef void (*Callback)(InterruptPin* pin, void* env);

private:
  Callback _callback;
  void* _env;

public:
  static InterruptPin* ext[2];
  
  enum Mode {
    ON_CHANGE_MODE = _BV(ISC00),
    ON_FALLING_MODE = _BV(ISC01),
    ON_RISING_MODE = (_BV(ISC01) | _BV(ISC00)),
    PULLUP_MODE = 4
  };

  /**
   * Construct interrupt pin with given pin number, mode, callback and environment.
   * @param[in] pin pin number.
   * @param[in] mode pin mode.
   * @param[in] fn callback function.
   * @param[in] env callback environment. 
   */
  InterruptPin(uint8_t pin, Mode mode = ON_CHANGE_MODE, Callback fn = 0, void* env = 0) :
    InputPin(pin), 
    _callback(fn),
    _env(env)
  {
    if (mode & PULLUP_MODE) {
      *PORT() |= _mask; 
    }
    if (pin > 1 && pin < 4) {
      pin = pin - 2;
      ext[pin] = this;
      bit_field_set(EICRA, bit_mask(pin), 2, bit_mask_get(mode, 0b11));
    }
  }

  /**
   * Set interrupt pin callback.
   * @param[in] fn callback function.
   * @param[in] env callback environment.
   */
  void set(Callback fn, void* env) { _callback = fn; _env = env; }

  /**
   * Enable interrupt pin change detection and callback.
   */
  void enable() { bit_set(EIMSK, _pin - 2); }

  /**
   * Disable interrupt pin change detection and callback.
   */
  void disable() { bit_clear(EIMSK, _pin - 2); }

  /**
   * Trampoline function for interrupt service on pin change interrupt.
   */
  void on_interrupt() { if (_callback != 0) _callback(this, _env); }

  /**
   * Callback function to push event for interrupt pin change.
   * @param[in] fn callback function.
   * @param[in] env callback environment.
   */
  static void push_event(InterruptPin* pin, void* env) 
  { 
    Event::push(Event::INTERRUPT_PIN_CHANGE_TYPE, pin, (uint16_t) env);
  }
};

/**
 * Abstract output pin. 
 */
class OutputPin : public Pin {

public:
  /**
   * Construct an abstract output pin for given Arduino pin number.
   * @param[in] pin number.
   * @param[in] initial value.
   */
  OutputPin(uint8_t pin, uint8_t initial = 0) : Pin(pin) 
  { 
    *DDR() |= _mask; 
    if (initial) set(); else clear();
  }

  /**
   * Set the output pin.
   */
  void set() { *PORT() |= _mask; }

  /**
   * Clear the output pin.
   */
  void clear() { *PORT() &= ~_mask; }

  /**
   * Toggle the output pin.
   */
  void toggle() { *PIN() |= _mask; }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set.
   * @param[in] value to set.
   */
  void set(uint8_t value) { if (value) set(); else clear(); }

  /**
   * Toggle the output pin to form a pulse with given length in
   * micro-seconds.
   * @param[in] us pulse width in micro seconds
   */
  void pulse(uint16_t us);
};

/**
 * Abstract pulse width modulation pin.
 */
class PWMPin : public OutputPin {

public:
  /**
   * Construct an abstract pwm output pin for given Arduino pin number.
   * @param[in] pin number.
   * @param[in] duty cycle (0..255)
   */
  PWMPin(uint8_t pin, uint8_t duty = 0) : OutputPin(pin) { set(duty); }

  /**
   * Set duty cycle for pwm output pin.
   * @param[in] duty cycle (0..255)
   */
  void set(uint8_t duty);

  /**
   * Set duty cycle for pwm output pin with given value mapping.
   * The value is mapped from ]min..max[ to duty [0..255]. Value
   * below min is mapped to zero(0) and above max to 255.
   * @param[in] value.
   * @param[in] min value.
   * @param[in] max value.
   */
  void set(uint16_t value, uint16_t min, uint16_t max);

  /**
   * Return duty setting for pwm output pin.
   * @return duty
   */
  uint8_t get_duty();
};

/*
 * Abstract analog pin. Allows asynchronous sampling.
 */
class AnalogPin : public Pin {

public:
  /**
   * Callback function prototype for analog sample completion.
   * @param[in] pin analog source.
   * @param[in] env callback environment.
   */
  typedef void (*Callback)(AnalogPin* pin, void* env);

  /**
   * Reference voltage; ARef pin, Vcc or internal 1V1.
   */
  enum Reference {
    APIN_REFERENCE = 0,
    AVCC_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = (_BV(REFS1) | _BV(REFS0))
  };

private:
  Reference _reference;
  Callback _callback;
  uint16_t _value;
  void* _env;
  
public:
  static AnalogPin* response;

  /**
   * Construct abstract analog pin for given Arduino pin with reference and
   * conversion completion callback function.
   * @param[in] pin number.
   * @param[in] ref reference voltage.
   * @param[in] fn conversion completion callback function.
   * @param[in] env callback environment. 
   */
  AnalogPin(uint8_t pin, Reference ref = AVCC_REFERENCE, Callback fn = 0, void* env = 0) : 
    Pin(pin < 14 ? pin + 14 : pin), 
    _reference(ref),
    _callback(fn),
    _value(0),
    _env(env)
  {
  }

  /**
   * Set conversion completion callback function.
   * @param[in] fn conversion completion callback function.
   * @param[in] env callback environment. 
   */
  void set(Callback fn, void* env = 0) { _callback = fn; _env = env; }

  /**
   * Set reference voltage for conversion.
   * @param[in] ref reference voltage.
   */
  void set(Reference ref) { _reference = ref; }

  /**
   * Get latest sample. 
   * @return sample value.
   */
  uint16_t get_value() { return (_value); }

  /**
   * Sample analog pin. Wait for conversion to complete before returning with
   * sample value.
   * @return sample value.
   */
  uint16_t sample();

  /**
   * Request sample of analog pin. Conversion completion function is called
   * if defined otherwise use await_sample().
   */
  void request_sample();

  /**
   * Await conversion to complete. Returns sample value
   * @return sample value.
   */
  uint16_t await_sample();

  /**
   * Trampoline function for interrupt service on conversion completion.
   * @param[in] value sample.
   */
  void on_sample(uint16_t value) 
  { 
    _value = value; 
    if (_callback != 0) _callback(this, _env); 
  }

  /**
   * Callback function to push event for sample conversion completion.
   * @param[in] pin analog pin source.
   * @param[in] value sample.
   */
  static void push_event(AnalogPin* pin, void* env)
  { 
    Event::push(Event::ANALOG_PIN_SAMPLE_TYPE, pin, (uint16_t) env);
  }
};

#endif
