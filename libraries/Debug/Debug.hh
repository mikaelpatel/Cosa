/**
 * @file Debug.hh
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

#ifndef COSA_DEBUG_HH
#define COSA_DEBUG_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

extern class Debug debug;

/**
 * Cosa Debug class with basic breakpoint, data observation and
 * display, and memory usage.
 *
 * @section Configuration
 * Define to remove corresponding command:
 *   COSA_DEBUG_NO_EXIT
 *   COSA_DEBUG_NO_WHERE
 *   COSA_DEBUG_NO_DUMP_VARIABLES
 *   COSA_DEBUG_NO_DUMP_DATA
 *   COSA_DEBUG_NO_DUMP_HEAP
 *   COSA_DEBUG_NO_DUMP_STACK
 *   COSA_DEBUG_NO_MEMORY_USAGE
 *   COSA_DEBUG_NO_HELP
 *   COSA_DEBUG_NO_LOOKUP_VARIABLES
 * May be used to reduce memory footprint and allow debugging with
 * limited resources.
 */
class Debug : public IOStream {
public:
  /**
   * Create debug iostream and command handler. Debug is a singleton.
   */
  Debug() :
    IOStream(),
    m_var(NULL),
    EXITCHARACTER(0x1d)
  {}

  /**
   * Start debug handler with given iostream device, file name, line
   * number and function name. The macro DEBUG_STREAM(dev) should be
   * used instead. Returns true(1) if successful otherwise false(0).
   * @param[in] dev iostream device for debug handler.
   * @param[in] file name.
   * @param[in] line number.
   * @param[in] func function name.
   * @return bool.
   */
  bool begin(IOStream::Device* dev,
	     const char* file,
	     int line,
	     const char* func);

  /**
   * Assertion failure in given file, on line, and in function. The
   * debug command handler is run. The macro ASSERT(cond) should
   * be used instead.
   * @param[in] file name.
   * @param[in] line number.
   * @param[in] func function name.
   * @param[in] cond condition string.
   */
  void assert(const char* file,
	      int line,
	      const char* func,
	      str_P cond)
  {
    print(PSTR("Debug::assert"));
    run(file, line, func, cond);
    print(PSTR("Debug::exit"));
    print(EXITCHARACTER);
    exit(0);
  }

  /**
   * Break point in given file, on line, and in function. The
   * debug command handler is run. The macro BREAKPOINT() or
   * BREAK_IF(cond) should be used instead.
   * @param[in] file name.
   * @param[in] line number.
   * @param[in] func function name.
   * @param[in] cond condition string.
   */
  void break_at(const char* file,
		int line,
		const char* func,
		str_P cond)
  {
    print(PSTR("Debug::break_at"));
    run(file, line, func, cond);
  }

  /**
   * Print variable obervation prefix with given file name, line
   * number and function name to debug stream. The macro
   * OBSERVE_IF(cond,expr) or OBSERVE(expr) should be used instead.
   * @param[in] file name.
   * @param[in] line number.
   * @param[in] func function name.
   * @param[in] expr expression string.
   */
  void observe_at(const char* file,
		  int line,
		  const char* func,
		  str_P expr)
  {
    UNUSED(file);
    printf(PSTR("Debug::observe_at:%s:%d:%S="), func, line, expr);
  }

  /**
   * Stop debug handler. Returns true(1) if successful otherwise false(0).
   * @return bool.
   */
  bool end();

  /**
   * Debug Variable information class. Contains function, variable
   * name, reference and size. Used by macro REGISTER(var) to allow
   * the debug handler to display variable values, addresses, etc.
   */
  class Variable {
  public:
    /**
     * Construct debug variable information instance and link into
     * debug handler list.
     */
    Variable(const char* func, str_P name, void* ref, size_t size) :
      m_next(debug.m_var),
      m_func(func),
      m_name(name),
      m_ref(ref),
      m_size(size)
    {
      debug.m_var = this;
    }

    /**
     * Unlink debug variable information instance from debug handler
     * list.
     */
    ~Variable()
    {
      debug.m_var = m_next;
    }

  protected:
    friend class Debug;
    class Variable* m_next;
    const char* m_func;
    str_P m_name;
    void* m_ref;
    size_t m_size;
  };

protected:
  /**
   * Run debug handler.
   * @param[in] file name (default NULL).
   * @param[in] line number (default 0).
   * @param[in] func function name (default NULL).
   * @param[in] expr expression string (default NULL).
   */
  void run(const char* file = NULL,
	   int line = 0,
	   const char* func = NULL,
	   str_P expr = NULL);

#if !defined(COSA_DEBUG_NO_EXIT)
  void do_exit();
#endif

#if !defined(COSA_DEBUG_NO_DUMP_VARIABLES)
  void do_dump_variables();
#endif

#if !defined(COSA_DEBUG_NO_DUMP_DATA)
  void do_dump_data();
#endif

#if !defined(COSA_DEBUG_NO_DUMP_HEAP)
  void do_dump_heap();
#endif

#if !defined(COSA_DEBUG_NO_DUMP_STACK)
  void do_dump_stack(int marker);
#endif

#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
  void do_memory_usage(int marker);
#endif

#if !defined(COSA_DEBUG_NO_HELP)
  void do_help();
#endif

  friend class Variable;
  Variable* m_var;
  char EXITCHARACTER;
  int DATAEND;
  int DATASIZE;
};

#if !defined(NDEBUG)

/**
 * Start the debug command handler with information about the file,
 * line number and function name.
 * @param[in] dev iostream device.
 */
#define DEBUG_STREAM(dev)						\
  do {									\
    debug.begin(&dev, __FILE__,__LINE__, __PRETTY_FUNCTION__);		\
  } while (0)

#if defined(ASSERT)
#undef ASSERT
#endif

/**
 * Assert the given condition. Calls debug handler if the assertion
 * fails. Will not return if asserted.
 * @param[in] cond condition.
 */
#define ASSERT(cond)							\
  do {									\
    if (UNLIKELY(!(cond)))						\
      debug.assert(__FILE__,__LINE__, __PRETTY_FUNCTION__,		\
		   __PSTR(# cond));					\
  } while (0)

/**
 * Call the debug command handler with information about the file,
 * line number and function name.
 */
#define BREAKPOINT()							\
  do {									\
    debug.break_at(__FILE__,__LINE__, __PRETTY_FUNCTION__, NULL);	\
  } while (0)

/**
 * Call the debug command handler with information about the file,
 * line number and function name if the given condition is true.
 * @param[in] cond condition.
 */
#define BREAK_IF(cond)							\
  do {									\
    if (UNLIKELY(cond))							\
      debug.break_at(__FILE__,__LINE__, __PRETTY_FUNCTION__,		\
                     __PSTR(# cond));					\
  } while (0)

/**
 * Print the given expression to the debug iostream if the condition
 * is true.
 * @param[in] cond condition.
 * @param[in] expr expression.
 */
#define OBSERVE_IF(cond,expr)						\
  do {									\
    if (UNLIKELY(cond)) {						\
      debug.observe_at(__FILE__,__LINE__, __PRETTY_FUNCTION__,		\
		       __PSTR(# expr));					\
      debug.print(expr);						\
      debug.println();							\
    }									\
  } while (0)

/**
 * Print the given expression to the debug iostream.
 * @param[in] expr expression.
 */
#define OBSERVE(expr) OBSERVE_IF(expr,true)

/**
 * Register the given variable in the debug handler.
 * @param[in] var variable.
 */
#define REGISTER(var)							\
  Debug::Variable debug__ ## var(__PRETTY_FUNCTION__,			\
				 __PSTR(#var),				\
				 (void*) &var,				\
				 sizeof(var));

#else

#define DEBUG_STREAM(dev)
#if !defined(ASSERT)
#define ASSERT(cond) do { if (!(cond)) exit(0); } while (0)
#endif
#define BREAKPOINT()
#define BREAK_IF(cond)
#define OBSERVE_IF(cond,expr)
#define OBSERVE(expr)
#define REGISTER(var)

#endif
#endif
