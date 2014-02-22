/**
 * @file Cosa/Nucleo/Semaphore.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_NUCLEO_SEMAPHORE_HH__
#define __COSA_NUCLEO_SEMAPHORE_HH__

#include "Cosa/Types.h"
#include "Cosa/Linkage.hh"

namespace Nucleo {

/**
 * The Cosa Nucleo Semaphore; counting synchronization.
 */
class Semaphore {
private:
  /** Queue for waiting threads */
  Head m_queue;

  /** Current count */
  volatile uint8_t m_count;

public:
  /**
   * Construct and initiate semaphore with given counter.
   * @param[in] count initial semaphore value (Default mutex, 1).
   */
  Semaphore(uint8_t count = 1) : m_queue(), m_count(count) {}

  /**
   * Wait for required count. Threads are queued until count is
   * available. 
   * @param[in] count requested count (Default mutex, 1).
   */
  void wait(uint8_t count = 1);

  /**
   * Signal release of given count. Waiting thread is resumed
   * after running thread has completed/yield.
   * @param[in] count released (Default mutex, 1).
   */
  void signal(uint8_t count = 1);
};

};
#endif
