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
 *   COSA_DEBUG_NO_BACKTRACE
 *   COSA_DEBUG_NO_HELP
 *   COSA_DEBUG_NO_LOOKUP_VARIABLES
 *   COSA_DEBUG_NO_MEMORY_USAGE
 *   COSA_DEBUG_NO_PRINT_DATA
 *   COSA_DEBUG_NO_PRINT_HEAP
 *   COSA_DEBUG_NO_PRINT_STACK
 *   COSA_DEBUG_NO_PRINT_VARIABLES
 *   COSA_DEBUG_NO_QUIT
 *   COSA_DEBUG_NO_WHERE
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
   * Assertion failure of given condition in source code (file, line
   * and function). The debug command handler is called. The macro
   * ASSERT(cond) should be used instead.
   * @param[in] file name.
   * @param[in] line number.
   * @param[in] func function name.
   * @param[in] cond condition string.
   */
  void assert(const char* file,
	      int line,
	      const char* func,
	      str_P cond);

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
		str_P cond);

  /**
   * Check stack status. Return false(0) if the stack has moved into
   * the heap else true(1).
   * @param[in] room required free bytes.
   * @return bool.
   */
  bool check_stack(int room = 128);

  /**
   * Print variable obervation prefix with given file name, line
   * number and function name to debug stream. The macro OBSERVE(expr)
   * or OBSERVE_IF(cond,expr) should be used instead.
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

    /**
     * Print information about variable on debug iostream device.
     */
    void print();

  protected:
    friend class Debug;
    class Variable* m_next;	//!< Next variable.
    const char* m_func;		//!< Registered in function.
    str_P m_name;		//!< Function name.
    void* m_ref;		//!< Variable value reference.
    size_t m_size;		//!< Variable value size.
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

#if !defined(COSA_DEBUG_NO_BACKTRACE)
  /**
   * Print backtrace.
   * @param[in] func debug entry function.
   */
  void do_backtrace(const char* func);
#endif

#if !defined(COSA_DEBUG_NO_HELP)
  /**
   * Print list of commands and short description.
   */
  void do_help();
#endif

#if !defined(COSA_DEBUG_NO_LOOKUP_VARIABLES)
  /**
   * Lookup given variable name in register. Return true(1) if found
   * otherwise false(0).
   * @param[in] name of variable.
   */
  bool do_lookup_variables(const char* name);
#endif

#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
  /**
   * Print memory usage statistics with size of data segment, heap,
   * stack and number of free bytes.
   */
  void do_memory_usage(int marker);
#endif

#if !defined(COSA_DEBUG_NO_PRINT_DATA)
  /**
   * Print contents of data segment in hex format.
   */
  void do_print_data();
#endif

#if !defined(COSA_DEBUG_NO_PRINT_HEAP)
  /**
   * Print contents of heap in hex format.
   */
  void do_print_heap();
#endif

#if !defined(COSA_DEBUG_NO_PRINT_STACK)
  /**
   * Print contents of stack in hex format.
   */
  void do_print_stack(int marker);
#endif

#if !defined(COSA_DEBUG_NO_PRINT_VARIABLES)
  /**
   * Print registered variables in format:
   * @code
   * REG:FUNC:VAR@REF=VAL
   * REG:FUNC:VAR@REF[SIZE]:HEX
   * @endcode
   * where REG is the stack address of register item for the variable VAR.
   * FUNC is the name of the function where registered. REF is the
   * address of the variables value and VAL is the value. If the value
   * is larger than int the value is printed in HEX format.
   */
  void do_print_variables();
#endif

#if !defined(COSA_DEBUG_NO_QUIT)
  /**
   * Stop the sketch.
   */
  void do_quit();
#endif

  friend class Variable;
  Variable* m_var;		//!< Last registered variable.
  char EXITCHARACTER;		//!< Character to emit on exit.
  int DATAEND;			//!< End of data segment.
  int DATASIZE;			//!< Size of data segment.
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
 * fails (the condition is false). Will not return if asserted.
 * @param[in] cond condition.
 */
#define ASSERT(cond)							\
  do {									\
    if (UNLIKELY(!(cond)))						\
      debug.assert(__FILE__,__LINE__, __PRETTY_FUNCTION__,		\
		   __PSTR(# cond));					\
  } while (0)

/**
 * Mark a break point in the source code. Call the debug command
 * handler with information about the file, line number and function
 * name.
 */
#define BREAKPOINT()							\
  do {									\
    debug.break_at(__FILE__,__LINE__, __PRETTY_FUNCTION__, NULL);	\
  } while (0)

/**
 * Mark a conditional break point in the source code. Call the debug
 * command handler with information about the file, line number and
 * function name if the given condition is true.
 * @param[in] cond condition.
 */
#define BREAK_IF(cond)							\
  do {									\
    if (UNLIKELY(cond))							\
      debug.break_at(__FILE__,__LINE__, __PRETTY_FUNCTION__,		\
                     __PSTR(# cond));					\
  } while (0)

/**
 * Check for stack error.
 * @param[in] room required free space (default 128).
 */
#define CHECK_STACK(room)						\
  do {									\
    if (UNLIKELY(!(debug.check_stack(room))))				\
      debug.assert(__FILE__,__LINE__, __PRETTY_FUNCTION__,		\
		   __PSTR("check_stack()"));				\
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
 * Register the given variable in the debug handler. The debug handler
 * commands "variables" and "?var" will print information about the
 * registered variable(s).
 * @param[in] var variable.
 */
#define REGISTER(var)							\
  Debug::Variable debug__ ## var(__PRETTY_FUNCTION__,			\
				 __PSTR(#var),				\
				 (void*) &var,				\
				 sizeof(var));

#else

#if !defined(ASSERT)
#define ASSERT(cond) do { if (!(cond)) exit(0); } while (0)
#endif
#define BREAKPOINT()
#define BREAK_IF(cond)
#define CHECK_STACK(room)
#define DEBUG_STREAM(dev)
#define OBSERVE_IF(cond,expr)
#define OBSERVE(expr)
#define REGISTER(var)

#endif
#endif
