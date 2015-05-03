/**
 * @file Nucleo/Semaphore.hh
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

#ifndef COSA_NUCLEO_SEMAPHORE_HH
#define COSA_NUCLEO_SEMAPHORE_HH

#include "Cosa/Types.h"
#include "Cosa/Linkage.hh"

namespace Nucleo {

/**
 * The Cosa Nucleo Semaphore; counting synchronization primitive.
 */
class Semaphore {
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
   * @param[in] flag resume waiting thread (Default true).
   */
  void signal(uint8_t count = 1, bool flag = true);

private:
  /** Queue for waiting threads. */
  Head m_queue;

  /** Current count. */
  volatile uint8_t m_count;
};

};
#endif
