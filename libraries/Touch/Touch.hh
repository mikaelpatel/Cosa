/**
 * @file Touch.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_TOUCH_HH
#define COSA_TOUCH_HH

#include "Cosa/Types.h"
#include "Cosa/IOPin.hh"
#include "Cosa/Periodic.hh"

/**
 * Touch Capacitive Sensor using periodic discharging to detect a
 * sensor touch. Uses the Cosa IOPin to allow changing of data direction
 * and the watchdog for periodic reading. A callback, virtual member
 * function is called on detection.
 *
 * @section Circuit
 * Connect a 1-10 M ohm pullup resistor to the selected pin.
 * @code
 * (VCC)---[1M]---+
 *                |
 *                |
 * (Dn)-----------+-----[]
 * @endcode
 */
class Touch : private IOPin, public Periodic {
public:
  /**
   * Create a touch capacitive sensor connected to the given pin.
   * @param[in] scheduler.
   * @param[in] pin identity.
   * @param[in] threshold time between release detect (Default 250 ms).
   */
  Touch(Job::Scheduler* scheduler, Board::DigitalPin pin, uint16_t threshold = 250);

  /**
   * @override{Touch}
   * Callback virtual member function; Should be implemented by sub-class.
   */
  virtual void on_touch() = 0;

protected:
  /** Pin sampling rate; watchdog timeout request. */
  static const uint16_t SAMPLE_RATE = 16;

  /** Minimum delay between release detect. */
  const uint16_t THRESHOLD;

  /** Start release detect. */
  uint32_t m_start;

  /** Discharging of sampling mode. */
  uint8_t m_sampling;

  /** Current state. */
  uint8_t m_touched;

  /**
   * @override{Job}
   * Called on watchdog timeout. Two state period function where the
   * io-pin is discharged and sampled.
   */
  virtual void run();
};

#endif
