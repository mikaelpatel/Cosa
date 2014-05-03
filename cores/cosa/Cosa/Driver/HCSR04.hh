/**
 * @file Cosa/Driver/HCSR04.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_DRIVER_HCSR04_HH
#define COSA_DRIVER_HCSR04_HH

#include "Cosa/Types.h"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Watchdog.hh"

/**
 * Device driver for Ultrasonic range module HC-SR04. Subclass
 * and implement the change event handler, on_change(). Attach
 * to watchdog timeout queue to perform periodic read and check 
 * of change.
 *
 * @section Circuit
 * @code
 *                           HC-SR04
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (D2)----------------2-|TRIG        |
 * (D3) ---------------3-|ECHO        |
 * (GND)---------------4-|GND         |
 *                       +------------+
 * @endcode
 * Connect HC-SR04 module to echo and trigger pin, and VCC and
 * ground. 
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read 
 * from the device. 
 */
class HCSR04 : private Link {
public:
  /**
   * Construct connection to a DHT11 device on given in/output-pin.
   * @param[in] trig_pin trigger pin number.
   * @param[in] echo_pin echo pin number.
   */
  HCSR04(Board::DigitalPin trig_pin, Board::DigitalPin echo_pin) :
    Link(),
    m_trigPin(trig_pin),
    m_echoPin(echo_pin),
    m_distance(0)
  {
  }
  
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
   * Schedule periodic reading with the given time period in milli-
   * seconds.
   * @param[in] ms milli-second sample period.
   */
  void periodic(uint16_t ms)
  {
    Watchdog::attach(this, ms);
  }
  
  /**
   * @override Periodic
   * Default on change function. Override for callback when the
   * distance has changed.
   * @param[in] distance in milli-meters.
   */
  virtual void on_change(uint16_t distance) 
  {
    UNUSED(distance);
  }

private:
  static const uint16_t TIMEOUT = 0xffffU;
  static const uint16_t COUNT_PER_DM = (555 * I_CPU) / 16;
  OutputPin m_trigPin;
  InputPin m_echoPin;
  uint16_t m_distance;

  /**
   * @override Event::Handler
   * Default device event handler function. Attach to watchdog
   * timer queue, Watchdog::attach(), to allow perodic reading
   * and check if the distance has changed.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
};

#endif
