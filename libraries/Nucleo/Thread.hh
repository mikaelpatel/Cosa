/**
 * @file Nucleo/Thread.hh
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

#ifndef COSA_NUCLEO_THREAD_HH
#define COSA_NUCLEO_THREAD_HH

#include "Cosa/Types.h"
#include "Cosa/Linkage.hh"
#include <setjmp.h>

namespace Nucleo {

/**
 * The Cosa Nucleo Thread; run-to-completion multi-tasking.
 */
class Thread : public Link {
public:
  /**
   * Return running thread.
   * @return thread.
   */
  static Thread* running()
  {
    return (s_running);
  }

  /**
   * Schedule static thread with given stack size. Using the default
   * parameters will start the main thread.
   * @param[in] thread to initiate and schedule.
   * @param[in] size of stack.
   */
  static void begin(Thread* thread = NULL, size_t size = 0);

  /**
   * @override{Nucleo::Thread}
   * The thread main function. The function is called when the thread
   * is scheduled and becomes running. Normally the function is an
   * end-less loop. Returning from the function will result in that
   * the function is called again. The default implementation is the
   * main thread. It is responsible for power down when there are no
   * other active threads. Other threads must override this member
   * function.
   */
  virtual void run();

  /**
   * Yield control to the given thread. Preserve stack and machine
   * state and later continue.
   * @param[in] thread to resume.
   */
  void resume(Thread* thread);

  /**
   * Yield control to the next thread in the thread queue. Preserve
   * stack and machine state and later continue.
   */
  void yield()
    __attribute__((always_inline))
  {
    Thread* thread = (Thread*) succ();
    if (thread == this) thread = &s_main;
    resume(thread);
  }

  /**
   * Delay at least the given time period in milli-seconds. The resolution
   * is determined by the Watchdog clock and has a resolution of 16
   * milli-seconds (per tick). The actual delay also depends on how
   * other threads yield.
   * @param[in] ms minimum delay time period in milli-seconds.
   */
  void delay(uint32_t ms);

  /**
   * Enqueue running thread to given queue and yield.
   * @param[in] queue to transfer to.
   * @param[in] thread to resume (Default yield).
   */
  void enqueue(Head* queue, Thread* thread = NULL);

  /**
   * If given queue is not empty dequeue first thread and resume
   * direct if flag is true otherwise enqueue first in run queue.
   * @param[in] queue to transfer from.
   * @param[in] flag resume direct otherwise on yield (Default true).
   */
  void dequeue(Head* queue, bool flag = true);

  /**
   * Service the nucleos main thread. Should be called in the
   * loop() function.
   */
  static void service();

protected:
  /** Size of main thread stack. */
  static const size_t MAIN_STACK_MAX = 64;

  /** Queue for delayed threads. */
  static Head s_delayed;

  /** Main thread and thread queue head. */
  static Thread s_main;

  /** Running thread. */
  static Thread* s_running;

  /** Top of stack allocation. */
  static size_t s_top;

  /** Thread context. */
  jmp_buf m_context;

  /** Delay time expires; should not run for more than 2**32 seconds. */
  uint32_t m_expires;

  /**
   * Initiate thread and prepare for initial call to virtual member
   * function run(). Stack frame is allocated by begin().
   * @param[in] stack top pointer.
   */
  void init(void* stack);

  /** Allow friends to use the queue member functions. */
  friend class Semaphore;
};

};
#endif
