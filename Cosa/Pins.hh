/**
 * @file Cosa/Pins.hh
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

#ifndef __COSA_PINS_HH__
#define __COSA_PINS_HH__

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/Event.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Board.hh"

class OutputPin;

class Pin {
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
   * Return pointer to Data Direction Register.
   * @return DDR register pointer.
   */
  volatile uint8_t* DDR() 
  { 
    return (m_sfr + 1); 
  }

  /**
   * Return pointer to data PORT register.
   * @return PORT register pointer.
   */
  volatile uint8_t* PORT() 
  { 
    return (m_sfr + 2); 
  }

  /**
   * Return pin change interrupt mask register.
   * @return pin change mask register pointer.
   */
  volatile uint8_t* PCIMR() 
  { 
    return (Board::PCIMR(m_pin));
  }

  /**
   * Return bit mask for given Arduino pin number.
   * @param[in] pin number.
   * @return pin bit mask.
   */
  static const uint8_t MASK(uint8_t pin)
  {
    return (_BV(Board::BIT(pin)));
  }

  /**
   * Return pointer to PIN register.
   * @param[in] pin number.
   * @return PIN register pointer.
   */
  static volatile uint8_t* PIN(uint8_t pin) 
  { 
    return (Board::SFR(pin));
  }

  /**
   * Return pointer to Data Direction Register.
   * @param[in] pin number.
   * @return DDR register pointer.
   */
  static volatile uint8_t* DDR(uint8_t pin) 
  { 
    return (Board::SFR(pin) + 1);
  }

  /**
   * Return pointer to data PORT register.
   * @param[in] pin number.
   * @return PORT register pointer.
   */
  static volatile uint8_t* PORT(uint8_t pin) 
  { 
    return (Board::SFR(pin) + 2);
  }

public:
  enum Direction {
    MSB_FIRST = 0, 
    LSB_FIRST = 1
  } __attribute__((packed));

  /**
   * Construct abstract pin given Arduino pin number.
   * @param[in] pin number.
   */
  Pin(uint8_t pin) : 
    m_sfr(Board::SFR(pin)), 
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
    return ((*PIN() & m_mask) != 0); 
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool is_on()
  { 
    return ((*PIN() & m_mask) != 0); 
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
    return ((*PIN() & m_mask) == 0); 
  }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  bool is_off() 
  { 
    return ((*PIN() & m_mask) == 0); 
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool read()
  { 
    return ((*PIN() & m_mask) != 0); 
  }

  /**
   * Shift in a byte from the input pin using the given clock
   * output pin. Shift in according to given direction.
   * @param[in] clk output pin.
   * @param[in] order bit first.
   * @return value.
   */
  uint8_t read(OutputPin& clk, Direction order = MSB_FIRST);

  /**
   * Use pin number directly to read value. Does not require an instance.
   * Return true(1) if the pin is set otherwise false(0).
   * @param[in] pin number.
   * @return boolean.
   */
  static bool read(uint8_t pin)
  {
    return ((*Board::SFR(pin) & MASK(pin)) != 0); 
  }

  /**
   * Read input pin and assign variable.
   * @param[out] var to assign.
   * @return pin.
   */
  Pin& operator>>(uint8_t& var)
  { 
    var = ((*PIN() & m_mask) != 0); 
    return (*this);
  }

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
  } __attribute__((packed));

  /**
   * Construct abstract input pin given Arduino pin number.
   * @param[in] pin number (digital pin).
   * @param[in] mode pin mode (default NORMAL_MODE).
   */
  InputPin(Board::DigitalPin pin, Mode mode = NORMAL_MODE) :
    Pin((uint8_t) pin)
  {
    if (mode == PULLUP_MODE) {
      synchronized {
	*PORT() |= m_mask; 
      }
    }
  }

  /**
   * Construct abstract input pin given Arduino pin number.
   * @param[in] pin number (analog pin).
   * @param[in] mode pin mode (default NORMAL_MODE).
   */
  InputPin(Board::AnalogPin pin, Mode mode = NORMAL_MODE) :
    Pin((uint8_t) pin)
  {
    if (mode == PULLUP_MODE) {
      synchronized {
	*PORT() |= m_mask; 
      }
    }
  }

  /**
   * Set input pin to given mode.
   * @param[in] pin number.
   * @param[in] mode pin mode (default NORMAL_MODE).
   */
  static void set_mode(uint8_t pin, Mode mode = NORMAL_MODE)
  {
    if (mode == PULLUP_MODE) {
      synchronized { 
	*PORT(pin) |= MASK(pin); 
      }
    }
  }
};

/**
 * Forward declare interrupt service routines to allow them 
 * as friends.
 */
extern "C" void INT0_vect(void) __attribute__ ((signal));
#if !defined(__ARDUINO_TINYX5__)
extern "C" void INT1_vect(void) __attribute__ ((signal));
#if !defined(__ARDUINO_STANDARD__)
extern "C" void INT2_vect(void) __attribute__ ((signal));
#if defined(__ARDUINO_MEGA__)
extern "C" void INT3_vect(void) __attribute__ ((signal));
extern "C" void INT4_vect(void) __attribute__ ((signal));
extern "C" void INT5_vect(void) __attribute__ ((signal));
#endif
#endif
#endif

/**
 * Abstract external interrupt pin. Allows interrupt handling on 
 * the pin value changes. 
 */
class ExternalInterruptPin : 
  public InputPin, 
  public Event::Handler, 
  public Interrupt::Handler {
private:
  static ExternalInterruptPin* ext[Board::EXT_MAX];
  uint8_t m_ix;

  /**
   * Interrupt handlers are friends.
   */
  friend void INT0_vect(void);
#if !defined(__ARDUINO_TINYX5__)
  friend void INT1_vect(void);
#if !defined(__ARDUINO_STANDARD__)
  friend void INT2_vect(void);
#if defined(__ARDUINO_MEGA__)
  friend void INT3_vect(void);
  friend void INT4_vect(void);
  friend void INT5_vect(void);
#endif
#endif
#endif

public:
  enum Mode {
    ON_LOW_LEVEL_MODE = 0,
    ON_CHANGE_MODE = _BV(ISC00),
    ON_FALLING_MODE = _BV(ISC01),
    ON_RISING_MODE = (_BV(ISC01) | _BV(ISC00)),
    PULLUP_MODE = 4
  } __attribute__((packed));

  /**
   * Construct external interrupt pin with given pin number and mode.
   * @param[in] pin pin number.
   * @param[in] mode pin mode.
   */
  ExternalInterruptPin(Board::ExternalInterruptPin pin, 
		       Mode mode = ON_CHANGE_MODE);

  /**
   * Enable interrupt pin change detection and interrupt handler.
   */
  void enable() 
  { 
    synchronized {
#if defined(__ARDUINO_TINYX5__)
      bit_set(GIMSK, INT0); 
#else
      bit_set(EIMSK, m_ix); 
#endif
    }
  }

  /**
   * Disable interrupt pin change detection.
   */
  void disable() 
  { 
    synchronized {
#if defined(__ARDUINO_TINYX5__)
      bit_clear(GIMSK, INT0);
#else
      bit_clear(EIMSK, m_ix); 
#endif
    }
  }

  /**
   * @override
   * Default interrupt service on external interrupt pin change.
   */
  virtual void on_interrupt();
};

/**
 * Forward declare interrupt service routines to allow them 
 * as friends.
 */
extern "C" void PCINT0_vect(void) __attribute__ ((signal));
#if !defined(__ARDUINO_TINYX5__)
extern "C" void PCINT1_vect(void) __attribute__ ((signal));
extern "C" void PCINT2_vect(void) __attribute__ ((signal));
#if defined(__ARDUINO_MIGHTY__)
extern "C" void PCINT3_vect(void) __attribute__ ((signal));
#endif
#endif

/**
 * Abstract interrupt pin. Allows interrupt handling on 
 * the pin value changes. 
 */
class InterruptPin : 
  public InputPin, 
  public Event::Handler, 
  public Interrupt::Handler {
private:
  static InterruptPin* pin[Board::PIN_MAX];
  static uint8_t state[Board::PCINT_MAX];

  /**
   * Interrupt handlers are friends.
   */
  friend void PCINT0_vect(void);
#if !defined(__ARDUINO_TINYX5__)
  friend void PCINT1_vect(void);
  friend void PCINT2_vect(void);
#if defined(__ARDUINO_MIGHTY__)
  friend void PCINT3_vect(void);
#endif

  /**
   * Map interrupt source: Check which pin(s) are the source of the
   * pin change interrupt and call the corresponding interrupt handler
   * per pin.
   * @param[in] ix port index.
   * @param8in] mask pin mask.
   */
  static void on_interrupt(uint8_t ix, uint8_t mask);
#endif
  
public:
  enum Mode {
    NORMAL_MODE = 0,
    PULLUP_MODE = 1
  } __attribute__((packed));

  /**
   * Start handling of pin change interrupt handling.
   */
  static void begin();

  /**
   * End handling of pin change interrupt handling.
   */
  static void end();

  /**
   * Construct interrupt pin with given pin number.
   * @param[in] pin pin number.
   * @param[in] mode pin mode.
   */
  InterruptPin(Board::InterruptPin pin, Mode mode = NORMAL_MODE) :
    InputPin((Board::DigitalPin) pin, (InputPin::Mode) mode) 
  {
  }

  /**
   * Enable interrupt pin change detection and interrupt handler.
   */
  void enable() 
  { 
    synchronized {
      *PCIMR() |= m_mask;
#if !defined(__ARDUINO_MEGA__)
      pin[m_pin] = this;
#else
      uint8_t ix = m_pin - (m_pin < 24 ? 24 : 48);
      pin[ix] = this;
#endif
    }
  }

  /**
   * Disable interrupt pin change detection.
   */
  void disable() 
  { 
    synchronized {
      *PCIMR() &= ~m_mask;
#if !defined(__ARDUINO_MEGA__)
      pin[m_pin] = 0;
#else
      uint8_t ix = m_pin - (m_pin < 24 ? 24 : 48);
      pin[ix] = 0;
#endif
    }
  }

  /**
   * @override
   * Default interrupt service on pin change interrupt.
   */
  virtual void on_interrupt();
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
  OutputPin(Board::DigitalPin pin, uint8_t initial = 0) : 
    Pin((uint8_t) pin) 
  { 
    synchronized {
      *DDR() |= m_mask; 
    }
    set(initial);
  }

  /**
   * Construct an abstract output pin for given Arduino pin number.
   * @param[in] pin number.
   * @param[in] initial value.
   */
  OutputPin(Board::AnalogPin pin, uint8_t initial = 0) : 
    Pin((uint8_t) pin) 
  { 
    synchronized {
      *DDR() |= m_mask; 
    }
    set(initial);
  }

  /**
   * Set output pin to mode.
   * @param[in] pin number.
   * @param[in] initial value.
   */
  static void set_mode(uint8_t pin, uint8_t initial = 0)
  {
    synchronized {
      *DDR(pin) |= MASK(pin); 
    }
    write(pin, initial);
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
    synchronized {
      *PORT() |= m_mask; 
    }
  }

  /**
   * Set the output pin.
   */
  void on()   
  { 
    synchronized {
      *PORT() |= m_mask; 
    }
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
    synchronized {
      *PORT() &= ~m_mask; 
    }
  }

  /**
   * Clear the output pin.
   */
  void off() 
  { 
    synchronized {
      *PORT() &= ~m_mask; 
    }
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
   * Toggle the output pin.
   * @param[in] pin number.
   */
  static void toggle(uint8_t pin) 
  { 
    synchronized {
      *PIN(pin) = MASK(pin); 
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set.
   * @param[in] value to set.
   */
  void set(bool value) 
  { 
    synchronized {
      if (value) {
	*PORT() |= m_mask; 
      }
      else {
	*PORT() &= ~m_mask; 
      }
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set.
   * @param[in] value to write.
   */
  void write(uint8_t value) 
  { 
    synchronized {
      if (value) {
	*PORT() |= m_mask; 
      }
      else {
	*PORT() &= ~m_mask; 
      }
    }
  }

  /**
   * Shift out given byte to the output pin using the given clock
   * output pin. Shift out according to given direction.
   * @param[in] clk output pin.
   * @param[in] value to write.
   * @param[in] order bit first.
   */
  void write(uint8_t value, OutputPin& clk, Direction order = MSB_FIRST);

  /**
   * Set the given output pin with the given value. Zero(0) to 
   * clear and non-zero to set.
   * @param[in] pin number.
   * @param[in] value to write.
   */
  static void write(uint8_t pin, uint8_t value) 
  { 
    volatile uint8_t* port = PORT(pin);
    const uint8_t mask = MASK(pin);
    synchronized {
      if (value) {
	*port |= mask;
      }
      else {
	*port &= ~mask;
      }
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set. Output operator syntax.
   * @param[in] value to write.
   */
  OutputPin& operator<<(uint8_t value)
  {
    set(value);
    return (*this);
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
  PWMPin(Board::PWMPin pin, uint8_t duty = 0) : 
    OutputPin((Board::DigitalPin) pin) 
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
   * Set duty cycle for pwm output pin.
   * @param[in] duty cycle (0..255)
   */
  PWMPin& operator<<(uint8_t duty)
  {
    set(duty);
    return (*this);
  }

  /**
   * Set duty cycle for pwm output pin with given value mapping.
   * The value is mapped from ]min..max[ to duty [0..255]. Value
   * below min is mapped to zero(0) and above max to 255.
   * @param[in] value to map.
   * @param[in] min value.
   * @param[in] max value.
   */
  void set(uint16_t value, uint16_t min, uint16_t max);

  /**
   * Set duty cycle for pwm output pin with given value mapping.
   * The value is mapped from ]min..max[ to duty [0..255]. Value
   * below min is mapped to zero(0) and above max to 255.
   * @param[in] value to map.
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
  } __attribute__((packed));

  /**
   * Construct abstract in/output pin given Arduino pin number.
   * @param[in] pin number.
   * @param[in] mode pin mode (normal or pullup).
   */
  IOPin(Board::DigitalPin pin, Mode mode = INPUT_MODE) : 
    OutputPin(pin),
    m_mode(mode)
  {
    set_mode(mode);
  }
  IOPin(Board::AnalogPin pin, Mode mode = INPUT_MODE) : 
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

/**
 * Forward declare interrupt service routines to allow them 
 * as friends.
 */
extern "C" void ADC_vect(void) __attribute__ ((signal));

/**
 * Abstract analog pin. Allows asynchronous sampling.
 */
class AnalogPin : 
  public Pin, 
  public Interrupt::Handler,
  public Event::Handler {
public:
  /**
   * Reference voltage; ARef pin, Vcc or internal 1V1.
   */
#if defined(__ARDUINO_TINYX5)
  enum Reference {
    AVCC_REFERENCE = 0,
    APIN_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = _BV(REFS1),
    A2V56_REFERENCE = (_BV(REFS2) | _BV(REFS1))
  } __attribute__((packed));
#else
  enum Reference {
    APIN_REFERENCE = 0,
    AVCC_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = (_BV(REFS1) | _BV(REFS0))
  } __attribute__((packed));
#endif

protected:
  static AnalogPin* sampling_pin;
  uint8_t m_reference;
  uint16_t m_value;
  uint8_t m_event;
  
  /**
   * Internal request sample of analog pin. Set up sampling of given pin
   * with given reference voltage.
   * @param[in] pin number.
   * @param[in] ref reference voltage.
   * @return bool.
   */
  bool sample_request(uint8_t pin, uint8_t ref);

  /**
   * @override
   * Handle analog pin periodic sampling and sample completed event.
   * Will call virtual method on_change() if the pin value has changed since
   * latest sample.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);

  /**
   * Interrupt handler is a friend.
   */
  friend void ADC_vect(void);

public:
  /**
   * Construct abstract analog pin for given Arduino pin with reference and
   * conversion completion interrupt handler.
   * @param[in] pin number.
   * @param[in] ref reference voltage.
   */
  AnalogPin(Board::AnalogPin pin, Reference ref = AVCC_REFERENCE) :
    Pin((uint8_t) pin),
    m_reference(ref),
    m_value(0),
    m_event(Event::NULL_TYPE)
  {
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
   * Sample analog pin. Wait for conversion to complete before 
   * returning with sample value.
   * @param[in] pin number.
   * @param[in] ref reference voltage.
   * @return sample value.
   */
  static uint16_t sample(uint8_t pin, Reference ref = AVCC_REFERENCE);

  /**
   * Sample analog pin. Wait for conversion to complete before 
   * returning with sample value.
   * @return sample value.
   */
  uint16_t sample()
  {
    return (m_value = AnalogPin::sample(m_pin, (Reference) m_reference));
  }

  /**
   * Sample analog pin. Wait for conversion to complete before 
   * returning with sample value.
   * @param[out] var variable to receive the value.
   * @return analog pin.
   */
  AnalogPin& operator>>(uint16_t& var)
  { 
    var = sample();
    return (*this);
  }

  /**
   * Request sample of analog pin. Pushes given event on completion. 
   * Default event is null/no event pushed for sample_await().
   * @param[in] event to push on completion.
   * @return bool.
   */
  bool sample_request(uint8_t event = Event::NULL_TYPE)
  {
    m_event = event;
    return (sample_request(m_pin, (Reference) m_reference));
  }

  /**
   * Await conversion to complete. Returns sample value
   * @return sample value.
   */
  uint16_t sample_await();

  /**
   * @override
   * Interrupt service on conversion completion.
   * @param[in] value sample.
   */
  virtual void on_interrupt(uint16_t value);

  /**
   * @override
   * Default on change function. 
   * @param[in] value.
   */
  virtual void on_change(uint16_t value) {}
};

/**
 * Abstract analog pin set. Allow sampling of a set of pins with
 * interrupt or event handler when completed.
 */
class AnalogPins : private AnalogPin {
private:
  const Board::AnalogPin* m_pin_at;
  uint16_t* m_buffer;
  uint8_t m_count;
  uint8_t m_next;

public:
  /**
   * Construct abstract analog pin set given vector and number of pins,
   * interrupt handler and environment. The vector of pins should be 
   * defined in program memory using PROGMEM.
   * @param[in] pins vector with analog pins.
   * @param[in] count number of analog pins in set.
   */
  AnalogPins(const Board::AnalogPin* pins, 
	     uint16_t* buffer, uint8_t count,
	     Reference ref = AVCC_REFERENCE) :
    AnalogPin((Board::AnalogPin) 255, ref),
    m_pin_at(pins),
    m_buffer(buffer != 0 ? 
	     buffer : 
	     (uint16_t*) malloc(sizeof(uint16_t) * count)),
    m_count(count),
    m_next(0)
  {
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
   * @return pin number.
   */
  uint8_t get_pin_at(uint8_t ix) 
  { 
    return (ix < m_count ? pgm_read_byte(&m_pin_at[ix]) : 0);
  }
  
  /**
   * Request sample of analog pin set. 
   */
  bool samples_request();

  /**
   * @override
   * Interrupt service on conversion completion.
   * @param[in] value sample.
   */
  virtual void on_interrupt(uint16_t value);

  /**
   * @override
   * Default analog pin set event handler function. 
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value) {}
};

/**
 * Forward declare interrupt service routines to allow them 
 * as friends.
 */
extern "C" void ANALOG_COMP_vect(void) __attribute__ ((signal));

/**
 * Analog Comparator; compare input values on the positive pin AIN0 
 * and negative pin AIN1 or ADCn. Note: only one instance can be
 * active/enabled at a time.
 */
class AnalogComparator : 
  public Interrupt::Handler, 
  public Event::Handler {
public:
  enum Mode {
    ON_TOGGLE_MODE = 0,
    ON_FALLING_MODE = _BV(ACIS1),
    ON_RISING_MODE = (_BV(ACIS1) | _BV(ACIS0)),
  } __attribute__((packed));

protected:
  static AnalogComparator* comparator;
  static const uint8_t AIN1 = 255;
  Mode m_mode;
  uint8_t m_pin;

  /**
   * Interrupt handler is a friend.
   */
  friend void ANALOG_COMP_vect(void);

public:
  /**
   * Construct analog comparator handler. Compare with AIN1.
   * @param[in] mode comparator mode.
   */
  AnalogComparator(Mode mode = ON_TOGGLE_MODE) :
    m_mode(mode),
    m_pin(AIN1)
  {
  }

  /**
   * Construct analog comparator handler. Compare with given 
   * analog pin (ADCn).
   * @param[in] pin analog pin to compare with.
   * @param[in] mode comparator mode.
   */
  AnalogComparator(Board::AnalogPin pin, Mode mode = ON_TOGGLE_MODE) :
    m_mode(mode),
    m_pin((uint8_t) (pin - Board::A0))
  {
  }

  /**
   * Enable analog comparator handler.
   */
  void enable()
  {
    synchronized {
      comparator = this;
      ADCSRB = _BV(ACME) | (m_pin == AIN1 ? _BV(ADEN) : m_pin);
      ACSR = _BV(ACIE) | m_mode;
    }
  }

  /**
   * Disable analog comparator handler.
   */
  void disable()
  {
    synchronized {
      bit_clear(ACSR, ACIE);
      comparator = 0;
    }
  }

  /**
   * @override
   * Default interrupt service on comparator output rise, fall or toggle.
   */
  virtual void on_interrupt();
};

#endif
