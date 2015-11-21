/**
 * @file HCSR04.hh
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

#ifndef COSA_HCSR04_HH
#define COSA_HCSR04_HH

#include "Cosa/Types.h"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Watchdog.hh"

/**
 * Device driver for Ultrasonic range module HC-SR04. Subclass and
 * implement the change event handler, on_change(). Attach to a
 * scheduler to perform periodic read and check of change.
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
class HCSR04 : public Periodic {
public:
  /**
   * Construct connection to a HC-SR04 device on given in/output-pin.
   * @param[in] trigger trigger pin.
   * @param[in] echo echo pin.
   */
  HCSR04(Job::Scheduler* scheduler,
	 Board::DigitalPin trigger,
	 Board::DigitalPin echo) :
    Periodic(scheduler, 250),
    m_trigger(trigger),
    m_echo(echo),
    m_distance(0)
  {}

  /**
   * Latest distance reading.
   * @return distance in millimeters.
   */
  uint16_t distance() const
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
  void schedule(uint16_t ms)
    __attribute__((always_inline))
  {
    stop();
    period(ms);
    expire_after(ms);
    start();
  }

  /**
   * @override{Periodic}
   * Default on change function. Override for callback when the
   * distance has changed.
   * @param[in] distance in milli-meters.
   */
  virtual void on_change(uint16_t distance)
  {
    UNUSED(distance);
  }

private:
  /** Timeout on failed to detect echo. */
  static const uint16_t TIMEOUT = 0xffffU;

  /** Count per decimeter. */
  static const uint16_t COUNT_PER_DM = (555 * I_CPU) / 16;

  /** Trigger output pin. */
  OutputPin m_trigger;

  /** Echo input pin. */
  InputPin m_echo;

  /** Latest valid distance. */
  uint16_t m_distance;

  /**
   * @override{Event::Handler}
   * Default device event handler function. Attach to scheduler to
   * allow perodic reading and check if the distance has changed.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
};

#endif
