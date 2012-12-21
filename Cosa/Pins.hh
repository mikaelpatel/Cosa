/**
 * @file Cosa/Pins.hh
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

#ifndef __COSA_PINS_HH__
#define __COSA_PINS_HH__

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/Thing.hh"
#include "Cosa/Event.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"

class Pin : public Thing {

protected:
  volatile uint8_t* const m_sfr;
  const uint8_t m_mask;
  const uint8_t m_pin;

  /**
   * Return pointer to PIN register.
   * @return PIN register pointer.
   */
  volatile uint8_t* PIN() 
  { 
    return (m_sfr); 
  }

  /**
   * Return pointer to data direction register.
   * @return DDR register pointer.
   */
  volatile uint8_t* DDR() 
  { 
    return (m_sfr + 1); 
  }

  /**
   * Return pointer to data port register.
   * @return PORT register pointer.
   */
  volatile uint8_t* PORT() 
  { 
    return (m_sfr + 2); 
  }

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
  Pin(uint8_t pin) : 
    Thing(), 
    m_sfr(SFR(pin)), 
    m_mask(MASK(pin)), 
    m_pin(pin) 
  {}

  /**
   * Return Arduino pin number of abstract pin.
   * @return pin number.
   */
  uint8_t get_pin() 
  { 
    return (m_pin); 
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool is_set() 
  { 
    return ((*PIN() & m_mask) != 0); 
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool is_high() 
  { 
    return (is_set()); 
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool is_on()
  { 
    return (is_set()); 
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool read()
  { 
    return (is_set()); 
  }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  bool is_clear() 
  { 
    return ((*PIN() & m_mask) == 0); 
  }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  bool is_low() 
  { 
    return (is_clear()); 
  }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  bool is_off() 
  { 
    return (is_clear()); 
  }

  /**
   * Await change of pin state given maximum number of micro seconds.
   * Returns number of wait cycles. 
   * @param[in] us micro seconds (1..255).
   * @return number of wait cycles.
   */
  uint8_t await_change(uint8_t us);

  /**
   * Print abstract pin information to given stream. Default is the
   * trace stream.  
   * @param[in] stream to print on.
   */
  void print(IOStream& stream = trace); 

  /**
   * Print abstract pin information to given stream with
   * new-line. Default is the trace stream. 
   * @param[in] stream to print on.
   */
  void println(IOStream& stream = trace);
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
  InputPin(uint8_t pin, Mode mode = NORMAL_MODE) :
    Pin(pin)
  {
    synchronized {
      if (mode == PULLUP_MODE) *PORT() |= m_mask; 
    }
  }
};

/**
 * Abstract interrupt pin. Allows interrupt handling on the pin value 
 * changes. 
 */
class InterruptPin : public InputPin {

public:
  /**
   * Interrupt handler function prototype.
   * @param[in] pin reference of changed pin.
   * @param[in] env interrupt handler environment. 
   */
  typedef void (*InterruptHandler)(InterruptPin* pin, void* env);

private:
  InterruptHandler m_handler;
  void* m_env;

public:
  static InterruptPin* ext[2];
  
  enum Mode {
    ON_CHANGE_MODE = _BV(ISC00),
    ON_FALLING_MODE = _BV(ISC01),
    ON_RISING_MODE = (_BV(ISC01) | _BV(ISC00)),
    PULLUP_MODE = 4
  };

  /**
   * Construct interrupt pin with given pin number, mode, interrupt
   * handler and environment.
   * @param[in] pin pin number.
   * @param[in] mode pin mode.
   * @param[in] fn interrupt handler function.
   * @param[in] env interrupt handler environment. 
   */
  InterruptPin(uint8_t pin, 
	       Mode mode = ON_CHANGE_MODE,  
	       InterruptHandler fn = 0, 
	       void* env = 0) :
    InputPin(pin), 
    m_handler(fn),
    m_env(env)
  {
    if (mode & PULLUP_MODE) {
      synchronized {
	*PORT() |= m_mask; 
      }
    }
    if (pin > 1 && pin < 4) {
      pin = pin - 2;
      ext[pin] = this;
      EICRA = (EICRA & ~(0b11 << pin)) | (mode << pin);
    }
  }

  /**
   * Set interrupt handler.
   * @param[in] fn interrupt handler function.
   * @param[in] env interrupt handler environment.
   */
  void set_interrupt_handler(InterruptHandler fn, void* env) 
  { 
    m_handler = fn; 
    m_env = env; 
  }

  /**
   * Enable interrupt pin change detection and interrupt handler.
   */
  void enable() 
  { 
    bit_set(EIMSK, m_pin - 2); 
  }

  /**
   * Disable interrupt pin change detection.
   */
  void disable() 
  { 
    bit_clear(EIMSK, m_pin - 2); 
  }

  /**
   * Trampoline function for interrupt service on pin change interrupt.
   */
  void on_interrupt() 
  { 
    if (m_handler != 0) m_handler(this, m_env); 
  }

  /**
   * Interrupt handler to push event for interrupt pin change.
   * @param[in] pin reference of changed pin.
   * @param[in] env interrupt/event handler environment. 
   */
  static void push_event(InterruptPin* pin, void* env) 
  { 
    Event::push(Event::CHANGE_TYPE, pin, env);
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
  OutputPin(uint8_t pin, uint8_t initial = 0) : 
    Pin(pin) 
  { 
    synchronized {
      *DDR() |= m_mask; 
    }
    if (initial) set(); else clear();
  }

  /**
   * Set the output pin.
   */
  void set() 
  { 
    synchronized {
      *PORT() |= m_mask; 
    }
  }

  /**
   * Set the output pin.
   */
  void high() 
  { 
    set(); 
  }

  /**
   * Set the output pin.
   */
  void on()   
  { 
    set(); 
  }

  /**
   * Clear the output pin.
   */
  void clear() 
  { 
    synchronized {
      *PORT() &= ~m_mask; 
    }
  }

  /**
   * Clear the output pin.
   */
  void low()
  { 
    clear(); 
  }

  /**
   * Clear the output pin.
   */
  void off() 
  { 
    clear(); 
  }

  /**
   * Toggle the output pin.
   */
  void toggle() 
  { 
    synchronized {
      *PIN() = m_mask; 
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set.
   * @param[in] value to set.
   */
  void set(uint8_t value) 
  { 
    if (value) set(); else clear(); 
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set.
   * @param[in] value to set.
   */
  void write(uint8_t value) 
  { 
    set(value); 
  }

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
  PWMPin(uint8_t pin, uint8_t duty = 0) : 
    OutputPin(pin) 
  { 
    set(duty); 
  }

  /**
   * Set duty cycle for pwm output pin.
   * @param[in] duty cycle (0..255)
   */
  void set(uint8_t duty);

  /**
   * Set duty cycle for pwm output pin.
   * @param[in] duty cycle (0..255)
   */
  void write(uint8_t duty) 
  { 
    set(duty); 
  }

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
   * Set duty cycle for pwm output pin with given value mapping.
   * The value is mapped from ]min..max[ to duty [0..255]. Value
   * below min is mapped to zero(0) and above max to 255.
   * @param[in] value.
   * @param[in] min value.
   * @param[in] max value.
   */
  void write(uint16_t value, uint16_t min, uint16_t max)
  {
    set(value, min, max);
  }

  /**
   * Return duty setting for pwm output pin.
   * @return duty
   */
  uint8_t get_duty();
};

/**
 * Abstract IO-pin that may switch between input and output pin.
 */
class IOPin : public OutputPin {

public:
  enum Mode {
    OUTPUT_MODE = 0,
    INPUT_MODE = 1,
    PULLUP_MODE = 2
  };

  /**
   * Construct abstract in/output pin given Arduino pin number.
   * @param[in] pin number.
   * @param[in] mode pin mode (normal or pullup).
   */
  IOPin(uint8_t pin, Mode mode = INPUT_MODE) : 
    OutputPin(pin),
    m_mode(mode)
  {
    set_mode(mode);
  }

  /**
   * Change IO-pin to given mode.
   * @param[in] mode new operation mode.
   */
  void set_mode(Mode mode)
  {
    synchronized {
      if (mode == OUTPUT_MODE)
	*DDR() |= m_mask; 
      else
	*DDR() &= ~m_mask; 
      if (mode == PULLUP_MODE)
	*PORT() |= m_mask; 
    }
    m_mode = mode;
  }
  
  /**
   * Get current IO-pin mode.
   * @return mode.
   */
  Mode get_mode()
  {
    return (m_mode);
  }
  
private:
  Mode m_mode;
};

/*
 * Abstract analog pin. Allows asynchronous sampling.
 */
class AnalogPin : public Pin {

public:
  /**
   * Interrupt handler prototype for analog sample completion.
   * @param[in] pin analog source.
   * @param[in] env interrupt handler environment.
   */
  typedef void (*InterruptHandler)(AnalogPin* pin, void* env);

  /**
   * Reference voltage; ARef pin, Vcc or internal 1V1.
   */
  enum Reference {
    APIN_REFERENCE = 0,
    AVCC_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = (_BV(REFS1) | _BV(REFS0))
  };

private:
  static AnalogPin* sampling_pin;
  Reference m_reference;
  InterruptHandler m_handler;
  uint16_t m_value;
  void* m_env;
  
public:
  /**
   * Construct abstract analog pin for given Arduino pin with reference and
   * conversion completion interrupt handler.
   * @param[in] pin number.
   * @param[in] ref reference voltage.
   * @param[in] fn conversion completion interrupt handler.
   * @param[in] env interrupt handler environment. 
   */
  AnalogPin(uint8_t pin, 
	    Reference ref = AVCC_REFERENCE, 
	    InterruptHandler fn = 0, 
	    void* env = 0) :
    Pin(pin < 14 ? pin + 14 : pin), 
    m_reference(ref),
    m_handler(fn),
    m_value(0),
    m_env(env)
  {
  }


  /**
   * Set conversion completion interrupt handler.
   * @param[in] fn conversion completion interrupt handler.
   * @param[in] env interrupt handler environment. 
   */
  void set_interrupt_handler(InterruptHandler fn, void* env = 0) 
  { 
    m_handler = fn; 
    m_env = env; 
  }

  /**
   * Set reference voltage for conversion.
   * @param[in] ref reference voltage.
   */
  void set_reference(Reference ref) 
  {
    m_reference = ref; 
  }

  /**
   * Get latest sample. 
   * @return sample value.
   */
  uint16_t get_value() 
  { 
    return (m_value); 
  }

  /**
   * Get reference to analog pin that is currently sampling.
   * Null if no conversion is active.
   * @return analog pin reference or null(0);
   */
  static AnalogPin* get_sampling_pin() 
  { 
    return (sampling_pin); 
  }

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
  void sample_request();

  /**
   * Await conversion to complete. Returns sample value
   * @return sample value.
   */
  uint16_t sample_await();

  /**
   * Trampoline function for interrupt service on conversion completion.
   * @param[in] value sample.
   */
  void on_sample(uint16_t value) 
  { 
    sampling_pin = 0;
    m_value = value; 
    if (m_handler != 0) m_handler(this, m_env); 
  }

  /**
   * Interrupt handler to push event for sample conversion completion.
   * @param[in] pin analog pin source.
   * @param[in] value sample.
   */
  static void push_event(AnalogPin* pin, void* env)
  { 
    Event::push(Event::SAMPLE_COMPLETED_TYPE, pin, env);
  }
};

/*
 * Abstract analog pin set. Allow sampling of a set of pins with
 * interrupt or event handler when completed.
 */
class AnalogPins : public Thing {

public:
  /**
   * Interrupt handler function prototype for analog pin set 
   * sample completion.
   * @param[in] set analog pin set.
   * @param[in] env interrupt handler environment.
   */
  typedef void (*InterruptHandler)(AnalogPins* set, void* env);

private:
  const AnalogPin** m_pin_at;
  InterruptHandler m_handler;
  uint8_t m_count;
  uint8_t m_next;
  void* m_env;

public:
  /**
   * Construct abstract analog pin set given vector and number of pins,
   * interrupt handler and environment. The vector of pins should be 
   * defined in program memory using PROGMEM.
   * @param[in] pins vector with analog pins.
   * @param[in] count number of analog pins in set.
   * @param[in] fn conversion completion interrupt handler.
   * @param[in] env interrupt handler environment.
   */
  AnalogPins(const AnalogPin** pins, 
	     uint8_t count, 
	     InterruptHandler fn = 0, 
	     void* env = 0) :
    m_pin_at(pins),
    m_handler(fn),
    m_count(count),
    m_env(env)
  {
    for (uint8_t ix = 0; ix < count; ix++)
      get_pin_at(ix)->set_interrupt_handler(sample_next, this);
  }
  
  /**
   * Get number of analog pins in set.
   * @return set size.
   */
  uint8_t get_count() 
  { 
    return (m_count); 
  }

  /**
   * Get analog pin in set. 
   * @param[in] ix index.
   * @return analog pin or null.
   */
  AnalogPin* get_pin_at(uint8_t ix) 
  { 
    return (ix < m_count ? (AnalogPin*) pgm_read_word(&m_pin_at[ix]) : 0);
  }

  /**
   * Start analog pin set sampling. All analog pins in set are
   * sampled in the background. Returns true(1) if started otherwise
   * false(0).
   * @param[in] fn conversion completion interrupt handler.
   * @param[in] env handler environment.
   * @return boolean.
   */
  bool samples_request(InterruptHandler fn = 0, void* env = 0);
  
  /**
   * Interrupt handler to push event for sample conversion completion.
   * @param[in] pin analog pin set.
   * @param[in] env handler environment.
   */
  static void push_event(AnalogPins* set, void* env)
  { 
    Event::push(Event::SAMPLE_COMPLETED_TYPE, set, env);
  }

 private:
  /**
   * Interrupt handler for analog pin completion to handle sampling of
   * the pins in the set. Will call the set interrupt handler when all
   * pins have been sampled and the conversions completed.
   * @param[in] pin analog pin.
   * @param[in] env analog pin set.
   */
  static void sample_next(AnalogPin* pin, void* env);
};

#endif
