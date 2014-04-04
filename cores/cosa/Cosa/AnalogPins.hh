/**
 * @file Cosa/AnalogPins.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#ifndef __COSA_ANALOG_PINS_HH__
#define __COSA_ANALOG_PINS_HH__

#include "Cosa/AnalogPin.hh"

/**
 * Abstract analog pin set. Allow sampling of a set of pins with
 * interrupt or event handler when completed.
 */
class AnalogPins : private AnalogPin {
public:
  /**
   * Construct abstract analog pin set given vector and number of pins,
   * interrupt handler and environment. The vector of pins should be 
   * defined in program memory using PROGMEM.
   * @param[in] pins vector with analog pins.
   * @param[in] buffer analog pin value storage.
   * @param[in] count number of pins in vector.
   * @param[in] ref reference voltage.
   */
  AnalogPins(const Board::AnalogPin* pins, 
	     uint16_t* buffer, uint8_t count,
	     Board::Reference ref = Board::AVCC_REFERENCE) :
    AnalogPin((Board::AnalogPin) 255, ref),
    m_pin_at(pins),
    m_buffer(buffer),
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
   * @override Interrupt::Handler
   * Interrupt service on conversion completion.
   * @param[in] arg sample value.
   */
  virtual void on_interrupt(uint16_t arg);

  /**
   * @override Event::Handler
   * Default analog pin set event handler function. 
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value) {}

private:
  const Board::AnalogPin* m_pin_at;
  uint16_t* m_buffer;
  uint8_t m_count;
  uint8_t m_next;
};

#endif
