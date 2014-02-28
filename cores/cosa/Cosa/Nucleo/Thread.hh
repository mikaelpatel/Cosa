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
 * The Cosa Nucleo Thread; run-to-completion multi-tasking. 
 */
class Thread : protected Link {
  friend class Semaphore;
private:
  /** Size of main thread stack */
  static const size_t MAIN_STACK_MAX = 32;

  /** Main thread and thread queue head */
  static Thread s_main;

  /** Running thread */
  static Thread* s_running;

  /** Top of stack allocation */
  static size_t s_top;

  /** Power down flag */
  static bool s_go_idle;

  /** Sleep mode */
  static uint8_t s_mode;

  /** Thread context */
  jmp_buf m_context;

  /** Thread state: 0=waiting, 1=runnable */
  uint8_t m_state;

  /**
   * Initiate thread with initial call to member function run(). 
   * Stack frame is allocated by begin().
   */
  void init();

  /**
   * Enqueue running thread to given queue and yield.
   * @param[in] queue to transfer to.
   */
  void enqueue(Head* queue);
  
  /**
   * If given queue is not empty dequeue first thread and resume
   * direct if flag is true otherwise on yield.
   * @param[in] queue to transfer from.
   * @param[in] flag resume direct otherwise on yield.
   */
  void dequeue(Head* queue, bool flag = true);
  
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
   * The thread main function. The function is called when the thread
   * is scheduled and becomes running. Normally the function is an
   * end-less loop. Returning from the function will result in that
   * the function is called again. The default implementation is the
   * main thread. It is responsible for power down when there are no
   * other active threads. 
   */
  virtual void run();

  /**
   * Yield control to the given thread. Preserve stack and machine 
   * state and later continue after this function.
   * @param[in] t thread to resume.
   */
  void resume(Thread* t);

  /**
   * Yield control to the next thread in the thread queue. Preserve
   * stack and machine state and later continue after this function. 
   */
  void yield() 
  { 
    resume((Thread*) get_succ()); 
  }

  /**
   * Delay at least the given time period in milli-seconds. The resolution
   * is determined by the Watchdog clock and has a minimum resolution of
   * 16 milli-seconds (per tick). The actual delay also depends on how
   * other threads yield control to other threads.
   * @param[in] ms minimum delay time period.
   */
  void delay(uint32_t ms);
};

};
#endif
