/**
 * @file Cosa/Interrupt.hh
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

#ifndef COSA_INTERRUPT_HH
#define COSA_INTERRUPT_HH

#include "Cosa/Types.h"

/**
 * Interface for Interrupt Handlers.
 */
class Interrupt {
public:
  class Handler {
  public:
    /**
     * @override{Interrupt::Handler}
     * Default interrupt handler. Should override.
     * @param[in] arg argument from interrupt service routine.
     */
    virtual void on_interrupt(uint16_t arg = 0)
    {
      UNUSED(arg);
    }

    /**
     * @override{Interrupt::Handler}
     * Enable interrupt handler.
     */
    virtual void enable() {}

    /**
     * @override{Interrupt::Handler}
     * Disable interrupt handler.
     */
    virtual void disable() {}

    /**
     * @override{Interrupt::Handler}
     * Clear interrupt.
     */
    virtual void clear() {}
  };
};
#endif

