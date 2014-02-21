/**
 * @file Cosa/Nucleo/Thread.hh
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

#ifndef __COSA_NUCLEO_THREAD_HH__
#define __COSA_NUCLEO_THREAD_HH__

#include "Cosa/Types.h"
#include "Cosa/Linkage.hh"
#include <setjmp.h>

namespace Nucleo {

/**
 * The Cosa Nucleo Thread; run-to-completion thread implementation.
 */
class Thread : protected Link {
private:
  /** Size of main thread stack */
  static const size_t MAIN_STACK_MAX = 32;

  /** Main thread and thread queue head */
  static Thread s_main;
  /** Top of stack allocation */
  static size_t s_top;

  /** Thread context */
  jmp_buf m_context;

  /**
   * Initiate thread with initial call to member function run(). 
   * Stack frame is allocated by begin().
   */
  void init();

public:
  /**
   * Schedule static thread with given stack size. Using the default
   * parameters will start the thread dispatcher.
   * @param[in] t thread to initiate and schedule.
   * @param[in] size of stack.
   */
  static void begin(Thread* t = NULL, size_t size = 0);

  /**
   * @override Nucleo::Thread
   * Thread function. Should be overridden. Default implementation
   * will yield.
   */
  virtual void run();

  /**
   * Yield control to the given thread.
   * @param[in] t thread to continue.
   */
  void resume(Thread* t);

  /**
   * Yield control to the next thread in the thread queue.
   */
  void yield() 
  { 
    resume((Thread*) get_succ()); 
  }

  /**
   * Delay at least the given time period in milli-seconds.
   * @param[in] ms time period.
   */
  void delay(uint32_t ms);
};

};
#endif
