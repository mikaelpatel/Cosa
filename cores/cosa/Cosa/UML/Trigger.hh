/**
 * @file Cosa/UML/Trigger.hh
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

#ifndef COSA_UML_TRIGGER_HH
#define COSA_UML_TRIGGER_HH

#include "Cosa/UML/Capsule.hh"
#include "Cosa/UML/Connector.hh"
#include "Cosa/ExternalInterrupt.hh"

namespace UML {

/**
 * Trigger Capsule class. Provides a boolean signal connector that is
 * set according to the trigger (external interrupt pin). A pullup
 * resistor is used and the falling edge of the input is detected by
 * default.
 *
 * @section Diagram
 * @code
 *
 *    Trigger
 *  +---------+
 *  | trigger |
 *  |         |---[Signal]--->
 *  |         |
 *  +---------+
 *  GND-()-[EXTn]
 *
 * @endcode
 */
class Trigger : public Capsule, public ExternalInterrupt {
public:
  /**
   * Construct Trigger on given external interrupt pin and generating
   * signal connector.
   * @param[in] pin digital pin for trigger.
   * @param[in] signal connector.
   * @param[in] mode of external interrupt.
   */
  Trigger(Board::ExternalInterruptPin pin, Signal& signal,
	  ExternalInterrupt::InterruptMode mode = ON_FALLING_MODE) :
    Capsule(),
    ExternalInterrupt(pin, mode, true),
    m_signal(signal)
  {}

protected:
  /**
   * @override Interrupt::Handler
   * Interrupt service callback on external interrupt pin change.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0)
  {
    UNUSED(arg);
    m_signal = read();
  }

  Signal& m_signal;		//!< Trigger output signal connector.
};

};

#endif
