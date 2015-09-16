/**
 * @file Cosa/Resource.hh
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

#ifndef COSA_RESOURCE_HH
#define COSA_RESOURCE_HH

#include "Cosa/Types.h"

/**
 * Abstract Resource Handler. Secure acquire and release of resources.
 */
class Resource {
public:
  /**
   * Construct Resource Handler.
   */
  Resource() :
    m_busy(false)
  {}

  /**
   * Acquire the resource. Wait until the resource is not busy.
   * Mark as busy and run call on_acquire() in mutual context.
   */
  void acquire()
  {
    uint8_t key = lock();
    while (UNLIKELY(m_busy)) {
      unlock(key);
      yield();
      key = lock();
    }
    m_busy = true;
    on_acquire();
    unlock(key);
  }

  /**
   * Release the resource. Call on_release() in mutex context.
   */
  void release()
  {
    uint8_t key = lock();
    on_release();
    m_busy = false;
    unlock(key);
  }

protected:
  /**
   * @override{Resource}
   * Resource extension on acquire.
   */
  virtual void on_acquire() = 0;

  /**
   * @override{Resource}
   * Resource extension on release.
   */
  virtual void on_release() = 0;

private:
  volatile bool m_busy;
};
#endif

