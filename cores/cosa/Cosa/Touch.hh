/**
 * @file Cosa/Touch.hh
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

#ifndef __COSA_TOUCH_HH__
#define __COSA_TOUCH_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/Linkage.hh"

/**
 * Touch Capacitive Sensor using periodic discharging to detect a 
 * sensor touch. Uses the Cosa IOPin to allow changing of data direction
 * and the watchdog for periodic reading. A callback, virtual member
 * function is called on detection. Connect a 1-10 M ohm pullup resistor
 * to the selected pin.
 */
class Touch : private IOPin, private Link {
protected:
  /** Pin sampling rate; watchdog timeout request */
  static const uint16_t SAMPLE_RATE = 16;

  /** Minimum delay between release detect */
  const uint16_t THRESHOLD;

  /** Start release detect */
  uint32_t m_start;

  /** Discharging of sampling mode */
  uint8_t m_sampling;

  /** Current state */
  uint8_t m_key_down;

  /**
   * @override
   * Called on watchdog timeout. Two state period function where the
   * io-pin is discharged and sampled.
   * @param[in] type the event type.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);

public:
  /**
   * Create a touch capacitive sensor connected to the given pin.
   * @param[in] pin identity.
   * @param[in] threshold time between release detect (Default 250 ms).
   */
  Touch(Board::DigitalPin pin, uint16_t threshold = 250);
  
  /**
   * @override
   * Callback virtual member function; Should be implemented by sub-class.
   */
  virtual void on_key_down() = 0;
};

#endif
