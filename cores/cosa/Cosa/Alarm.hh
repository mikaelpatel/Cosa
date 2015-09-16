/**
 * @file Cosa/Alarm.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_ALARM_HH
#define COSA_ALARM_HH

#include "Cosa/Types.h"
#include "Cosa/Clock.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/ExternalInterrupt.hh"

/**
 * The Alarm class is an extension of the Periodic job class to allow
 * repeated jobs with seconds as time unit. The abstract Clock is used
 * as the alarm scheduler.
 */
class Alarm : public Periodic {
public:
  /**
   * Alarm Clock is an external interrupt based job scheduler. The
   * interrupt source should provide an interrupt every second to
   * update the seconds counter.
   */
  class Clock : public ::Clock, public ExternalInterrupt {
  public:
    /**
     * Construct Alarm Clock with given external interrupt pin and
     * mode.
     * @param[in] pin number.
     * @param[in] mode pin mode (Default ON_RISING_MODE).
     * @param[in] pullup flag (Default false).
     */
    Clock(Board::ExternalInterruptPin pin,
	  InterruptMode mode = ON_RISING_MODE,
	  bool pullup = false) :
      ::Clock(),
      ExternalInterrupt(pin, mode, pullup)
    {}

  protected:
    /**
     * @override{Interrupt::Handler}
     * Increment the seconds counter and dispatch any expired alarms.
     * @param[in] arg argument from interrupt service routine (not used).
     */
    virtual void on_interrupt(uint16_t arg = 0)
    {
      UNUSED(arg);
      tick(1000);
    }
  };

  /**
   * Construct alarm with given clock (job scheduler) and timeout
   * period in seconds.
   * @param[in] clock for alarms.
   * @param[in] period seconds.
   */
  Alarm(::Clock* clock, uint32_t period) :
    Periodic(clock, period)
  {}
};
#endif

