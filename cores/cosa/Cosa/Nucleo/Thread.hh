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
class Thread : protected Link {
public:
  /**
   * Return running thread.
   * @return thread.
   */
  static Thread* get_running() 
  { 
    return (s_running); 
  }

  /**
   * Schedule static thread with given stack size. Using the default
   * parameters will start the thread dispatcher.
   * @param[in] thread to initiate and schedule.
   * @param[in] size of stack.
   */
  static void begin(Thread* thread = NULL, size_t size = 0);

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
  void resume(Thread* thread);

  /**
   * Yield control to the next thread in the thread queue. Preserve
   * stack and machine state and later continue after this function. 
   */
  void yield() __attribute__((always_inline))
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

  /**
   * Wait for a given bit to be set in the variable.
   * @param[in] ptr pointer to variable (uint8_t).
   * @param[in] bit position that should be set (default bit zero).
   */
  void await(volatile uint8_t* ptr, uint8_t bit = 0);

protected:
  /** Size of main thread stack */
  static const size_t MAIN_STACK_MAX = 64;

  /** Queue for delayed threads */
  static Head s_delayed;

  /** Main thread and thread queue head */
  static Thread s_main;

  /** Running thread */
  static Thread* s_running;

  /** Top of stack allocation */
  static size_t s_top;

  /** Thread context */
  jmp_buf m_context;

  /** Delay time expires; should not run for more than 2**32 seconds */
  uint32_t m_expires;

  /**
   * Initiate thread with initial call to member function run(). 
   * Stack frame is allocated by begin().
   * @param[in] stack top pointer.
   */
  void init(void* stack);

  /**
   * Enqueue running thread to given queue and yield.
   * @param[in] queue to transfer to.
   */
  void enqueue(Head* queue);
  
  /**
   * If given queue is not empty dequeue first thread and resume
   * direct if flag is true otherwise on yield.
   * @param[in] queue to transfer from.
   * @param[in] flag resume direct otherwise on yield (Default true).
   */
  void dequeue(Head* queue, bool flag = true);

  /** Allow friends to use the queue member functions */
  friend class Semaphore;
};

};
#endif
