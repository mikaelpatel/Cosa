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
   * Mark as busy and call on_acquire() in synchronized context.
   */
  void acquire()
  {
    uint8_t key = lock(m_busy);
    on_acquire();
    unlock(key);
  }

  /**
   * Try to acquire the resource. If successful, mark as busy and call
   * on_acquire() in synchronized context.
   * @return bool.
   */
  bool try_acquire()
  {
    synchronized {
      if (m_busy) return (false);
      m_busy = true;
      on_acquire();
    }
  }

  /**
   * Release the resource. Call on_release() in synchronized context
   * and mark the resource as available.
   */
  void release()
  {
    synchronized {
      on_release();
      m_busy = false;
    }
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

