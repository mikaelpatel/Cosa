/**
 * @file Cosa/Button.hh
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

#ifndef COSA_BUTTON_HH
#define COSA_BUTTON_HH

#include "Cosa/Types.h"
#include "Cosa/InputPin.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Watchdog.hh"

/**
 * Debounded Button; Sampled input pin (with internal pullup
 * resistor). Uses a periodic function with timout of 64 ms for
 * sampling and on change calls an event action. Subclass Button and
 * implement the virtual on_change() method. Use the subclass for any
 * state needed for the action function. Connect button/switch from
 * pin to ground. Internal pull-up resistor is activated.
 *
 * @section Circuit
 * @code
 * (Dn)-----------------+
 *                      |
 *                     (\)
 *                      |
 * (GND)----------------+
 * @endcode
 *
 * @section Limitations
 * Button toggle faster than sample period may be missed. This is the
 * case when connecting to a Rotary Encoder.
 *
 * @section See Also
 * The Button event handler requires the usage of an event dispatch or
 * implementing the virtual member function Job::on_expired() to call
 * run() and reschedule().
 * See Event.hh.
 */
class Button : public InputPin, public Periodic {
public:
  /**
   * Button change detection modes; falling (high to low), rising (low
   * to high) and change (falling or rising).
   */
  enum Mode {
    ON_FALLING_MODE = 0,	// High to low transition.
    ON_RISING_MODE = 1,		// Low to high transition.
    ON_CHANGE_MODE = 2		// Any transition.
  } __attribute__((packed));

  /**
   * Construct a button connected to the given pin and with
   * the given change detection mode. The scheduler should allow
   * periodic jobs with a time unit of milli-seconds
   * (e.g. Watchdog::Scheduler).
   * @param[in] scheduler for periodic job.
   * @param[in] pin number.
   * @param[in] mode change detection mode.
   */
  Button(Job::Scheduler* scheduler,
	 Board::DigitalPin pin,
	 Mode mode = ON_CHANGE_MODE) :
    InputPin(pin, InputPin::PULLUP_MODE),
    Periodic(scheduler, SAMPLE_MS),
    MODE(mode),
    m_state(is_set())
  {}

  /**
   * @override{Button}
   * The button change handler. Called when a change corresponding to
   * the mode has been detected. Event types are; Event::FALLING_TYPE,
   * Event::RISING_TYPE, and Event::CHANGE_TYPE. Sub-class must
   * override this method.
   * @param[in] type event type.
   */
  virtual void on_change(uint8_t type) = 0;

protected:
  /** Button sampling period in milli-seconds. */
  static const uint16_t SAMPLE_MS = 64;

  /** Change detection mode. */
  const Mode MODE;

  /** Current state. */
  uint8_t m_state;

  /**
   * @override{Job}
   * Button periodic function. Called by job scheduler on
   * timeout. Samples the  attached pin and calls the pin change
   * handler, on_change().
   */
  virtual void run()
  {
    // Update the button state
    uint8_t old_state = m_state;
    m_state = is_set();
    uint8_t new_state = m_state;

    // If changed according to mode call the pin change handler
    if ((old_state != new_state) &&
	((MODE == ON_CHANGE_MODE) || (new_state == MODE)))
      on_change(Event::FALLING_TYPE + MODE);
  }
};

#endif
