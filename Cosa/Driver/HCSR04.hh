/**
 * @file Cosa/Driver/HCSR04.hh
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
 * Device driver for Ultrasonic range module HC-SR04.
 *
 * @section Circuit
 * Connect HC-SR04 module to echo and trigger pin, and VCC and
 * ground. 
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read 
 * from the device. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_DRIVER_HCSR04_HH__
#define __COSA_DRIVER_HCSR04_HH__

#include "Cosa/Types.h"
#include "Cosa/Thing.hh"
#include "Cosa/Pins.hh"

class HCSR04 : public Thing {
private:
  static const uint16_t TIMEOUT = 0xffffU;
  static const uint16_t COUNT_PER_CM = 54;
  OutputPin m_trigPin;
  InputPin m_echoPin;
  uint16_t m_distance;
public:
  /**
   * Construct connection to a DHT11 device on given in/output-pin.
   * @param[pin] trig_pin trigger pin number.
   * @param[pin] echo_pin echo pin number.
   */
  HCSR04(uint8_t trig_pin, uint8_t echo_pin) :
    Thing(),
    m_trigPin(trig_pin),
    m_echoPin(echo_pin),
    m_distance(0)
  {}

  /**
   * Latest distance reading.
   * @return distance in millimeters.
   */
  uint16_t get_distance()
  {
    return (m_distance);
  }

  /**
   * Read distance in millimeter from device. Return true(1) if 
   * successful otherwise false(0).
   * @param[out] distance reading.
   * @return bool.
   */
  bool read(uint16_t& distance);

  /**
   * @override
   * Default device event handler function. Attach to watchdog
   * timer queue, Watchdog::attach(), to allow perodic reading.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    read(m_distance);
  }
};

#endif
