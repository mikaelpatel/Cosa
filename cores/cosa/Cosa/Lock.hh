/**
 * @file Cosa/Lock.hh
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

#ifndef COSA_LOCK_HH
#define COSA_LOCK_HH

#include "Cosa/Types.h"

/**
 * Lock/Unlock class. Alternative to synchronized block and
 * lock/unlock functions. Used in the form:
 * @code
 * {
 *   Lock key;
 *   ...
 *   if (...) return;
 *   if (...) goto label;
 * }
 * label:
 * @endcode
 * Interrupts are disabled in the block allowing secure update.
 */
class Lock {
public:
  /**
   * Construct lock and turn off interrupt handlers. Save processor
   * state.
   */
  Lock()
  {
    m_key = SREG;
    __asm__ __volatile__("cli" ::: "memory");
  }

  /**
   * Destruct lock and restore processor state.
   */
  ~Lock()
  {
    SREG = m_key;
    __asm__ __volatile__("" ::: "memory");
  }

private:
  /** Processor state to restore. */
  uint8_t m_key;
};
#endif

