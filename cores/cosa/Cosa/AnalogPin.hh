/**
 * @file Cosa/AnalogPin.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_ANALOG_PIN_HH
#define COSA_ANALOG_PIN_HH

#include "Cosa/Interrupt.hh"
#include "Cosa/Event.hh"
#include "Cosa/Types.h"

/**
 * Abstract analog pin. Allows asynchronous sampling with interrupt
 * and event handler.
 */
class AnalogPin : public Interrupt::Handler, public Event::Handler {
public:
  /**
   * Construct abstract analog pin for given pin (channel) and
   * reference voltage.
   * @param[in] pin number.
   * @param[in] ref reference voltage (default VCC).
   */
  AnalogPin(Board::AnalogPin pin, Board::Reference ref = Board::AVCC_REFERENCE) :
    m_pin(pin),
    m_reference(ref),
    m_value(0),
    m_event(Event::NULL_TYPE)
  {}

  /**
   * Set reference voltage for conversion.
   * @param[in] ref reference voltage.
   */
  void reference(Board::Reference ref)
  {
    m_reference = ref;
  }

  /**
   * Get reference voltage for conversion.
   * @return reference voltage.
   */
  Board::Reference reference() const
  {
    return (m_reference);
  }

  /**
   * Get analog pin.
   * @return pin identity.
   */
  Board::AnalogPin pin() const
  {
    return (m_pin);
  }

  /**
   * Get latest sample.
   * @return sample value.
   * @note atomic
   */
  uint16_t value() const
  {
    uint16_t res;
    synchronized res = m_value;
    return (res);
  }

  /**
   * Set analog conversion clock prescale (2..128, step power 2).
   * @param[in] factor prescale.
   */
  static void prescale(uint8_t factor);

  /**
   * Sample analog pin. Wait for conversion to complete before
   * returning with sample value.
   * @param[in] pin number.
   * @param[in] ref reference voltage.
   * @return sample value.
   */
  static uint16_t sample(Board::AnalogPin pin,
			 Board::Reference ref = Board::AVCC_REFERENCE);

  /**
   * Get power supply voltage in milli-volt. May be used for low battery
   * detection. Uses the internal 1V1 bandgap reference.
   * @param[in] vref reference voltage in milli-volt (default is 1100 mv).
   * @return milli-volt.
   */
  static uint16_t bandgap(uint16_t vref = 1100);

  /**
   * Enable analog conversion.
   */
  static void powerup()
    __attribute__((always_inline))
  {
    Power::adc_enable();
    bit_set(ADCSRA, ADEN);
  }

  /**
   * Disable analog conversion.
   */
  static void powerdown()
    __attribute__((always_inline))
  {
    bit_clear(ADCSRA, ADEN);
    Power::adc_disable();
  }

  /**
   * Sample analog pin. Wait for conversion to complete before
   * returning with sample value.
   * @return sample value.
   */
  uint16_t sample()
    __attribute__((always_inline))
  {
    return (m_value = AnalogPin::sample(m_pin, m_reference));
  }

  /**
   * Sample analog pin. Wait for conversion to complete before
   * returning with sample value.
   * @param[out] var variable to receive the value.
   * @return analog pin.
   */
  AnalogPin& operator>>(uint16_t& var)
    __attribute__((always_inline))
  {
    var = sample();
    return (*this);
  }

  /**
   * Sample analog pin. Wait for conversion to complete before
   * returning with sample value.
   * @return sample value.
   */
  operator uint16_t()
    __attribute__((always_inline))
  {
    return (sample());
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
    return (sample_request(m_pin, (Board::Reference) m_reference));
  }

  /**
   * Await conversion to complete. Returns sample value
   * @return sample value.
   */
  uint16_t sample_await();

  /**
   * @override{Interrupt::Handler}
   * Interrupt service on conversion completion.
   * @param[in] arg sample value.
   */
  virtual void on_interrupt(uint16_t arg);

  /**
   * @override{AnalogPin}
   * Default on change function.
   * @param[in] value.
   */
  virtual void on_change(uint16_t value)
  {
    UNUSED(value);
  }

protected:
  static AnalogPin* sampling_pin; //!< Current sampling pin if any.
  const Board::AnalogPin m_pin;	  //!< Analog channel number.
  Board::Reference m_reference;	  //!< ADC reference voltage type.
  uint16_t m_value;		  //!< Latest sample value.
  uint8_t m_event;		  //!< Event to push on completion.

  /**
   * Internal request sample of analog pin. Set up sampling of given pin
   * with given reference voltage.
   * @param[in] pin number.
   * @param[in] ref reference voltage.
   * @return bool.
   */
  bool sample_request(Board::AnalogPin pin, uint8_t ref);

  /**
   * @override{Event::Handler}
   * Handle analog pin periodic sampling and sample completed event.
   * Will call virtual method on_change() if the pin value has changed since
   * latest sample.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);

  /** Interrupt Service Routine */
  friend void ADC_vect(void);
};

#endif
