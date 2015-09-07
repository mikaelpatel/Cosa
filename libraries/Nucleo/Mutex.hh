/**
 * @file Nucleo/Mutex.hh
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

#ifndef COSA_NUCLEO_MUTEX_HH
#define COSA_NUCLEO_MUTEX_HH

#include "Semaphore.hh"

namespace Nucleo {

/**
 * The Cosa Nucleo Mutex; mutual exclusion block. Used as a local
 * variable in a block to wait and signal a semaphore to achive mutual
 * exclusive execution of the block.
 */
class Mutex {
public:
  /**
   * Start mutual exclusion block using given semaphore. The semaphore
   * should  be initiated with one (which is also the default value).
   * @param[in] sem semaphore to wait.
   */
  Mutex(Semaphore& sem) : m_sem(sem) { m_sem.wait(); }

  /**
   * End of mutual exclusion block. Will signal semaphore.
   */
  ~Mutex() { m_sem.signal(); }

private:
  Semaphore& m_sem;
};

};

/**
 * Syntactic sugar for mutual exclusive block. Used in the form:
 * @code
 * Nucleo::Semaphore s(1);
 * ...
 * mutex(s) {
 *   ...
 * }
 * @endcode
 */
#define mutex(s) for (uint8_t i = (s.wait(), 1); i != 0; i--, s.signal())

#endif
