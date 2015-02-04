/**
 * @file UML/Button.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_UML_BUTTON_HH
#define COSA_UML_BUTTON_HH

#include "UML/Capsule.hh"
#include "UML/Connector.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/InputPin.hh"

namespace UML {

/**
 * Button Capsule class. Provides a boolean signal connector that is
 * set according to the button (digital pin). The pin is periodically
 * sampled and listeners are scheduled when the state changes. 
 */
class Button : public Capsule, public Periodic, private InputPin {
public:
  /**
   * Type of button signal connector.
   */
  typedef Connector<bool> Signal;

  /**
   * Construct Clock with given tick connector and period in
   * milli-seconds. 
   * @param[in] tick connector.
   * @param[in] ms period.
   */
  Button(Board::DigitalPin pin, Signal& signal) : 
    Capsule(), 
    Periodic(64),
    InputPin(pin, InputPin::PULLUP_MODE),
    m_signal(signal)
  {}

  /**
   * @override Capsule
   * Increment clock tick and schedule all capsules that listen for
   * clock update.
   */
  virtual void behavior() 
  {
    bool value = read();
    if (m_signal == value) return;
    m_signal = value;
  }

  /**
   * @override Periodic
   * Schedule this capsule on timeout. Could also call behaviour
   * directly.
   */
  virtual void run()
  {
    controller.schedule(this);
  }

protected:
  Signal& m_signal;
};

};

#endif
