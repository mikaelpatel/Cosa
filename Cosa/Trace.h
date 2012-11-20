/**
 * @file Cosa/Trace.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
 * @section Description
 * Basic trace support class. Combind IOStream with UART for trace
 * output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TRACE_H__
#define __COSA_TRACE_H__

#include "Cosa/Types.h"
#include "Cosa/IOStream.h"
#include "Cosa/UART.h"

class Trace : public IOStream {

private:
  UART uart;

public:
  /**
   * Construct Trace IOStream object and initiate UART object.
   * The Trace class is actually a singleton; trace.
   */
  Trace() : IOStream(&uart) {}

  /**
   * Start trace stream over UART transmitter.
   * @param[in] baudrate serial bitrate.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint32_t baudrate = 9600)
  {
    return (uart.begin(baudrate));
  }

  /**
   * Stop trace stream over current device.
   * @return true(1) if successful otherwise false(0)
   */
  bool end()
  {
    return (uart.end());
  }
};

/**
 * The Trace class singleton. 
 */
extern Trace trace;

/**
 * Log priorities.
 */
#define	LOG_EMERG	0	/* System is unusable */
#define	LOG_ALERT	1	/* Action must be taken immediately */
#define	LOG_CRIT	2	/* Critical conditions */
#define	LOG_ERR		3	/* Error conditions */
#define	LOG_WARNING	4	/* Warning conditions */
#define	LOG_NOTICE	5	/* Normal but significant condition */
#define	LOG_INFO	6	/* Informational */
#define	LOG_DEBUG	7	/* Debug-level messages */

/**
 * Macros to generate mask for the trace log priorities (LOG_EMERG..
 * LOG_DEBUG) setting of trace_log_mask.
 * LOG_MASK(prio) bit mask corresponding to the given priority.
 * LOG_UPTO(prio) bit mask for all priorities including the given.
 */
#define LOG_MASK(prio) (1 << (prio))
#define LOG_UPTO(prio) (LOG_MASK((prio) + 1) - 1)
extern int8_t trace_log_mask;

#ifndef NDEBUG

/**
 * Support macro for trace of a string in program memory.
 * @param[in] str string literal
 */
# define TRACE_PSTR(str) trace.print_P(PSTR(str))

/**
 * Support macro for trace of an expression. The expression
 * is used as a string and the value is evaluated.
 * @param[in] expr expression.
 */
# define TRACE(expr)							\
  do {									\
    trace.print_P(PSTR(#expr " = "));					\
    trace.print(expr);							\
    trace.println();							\
  } while (0)

/**
 * Support macro for trace of a log message with line number and
 * function name prefix.
 * @param[in] msg log message.
 */
# define TRACE_LOG(msg, ...)						\
  trace.printf_P(PSTR("%d:%s:" msg "\n"),				\
		 __LINE__, __func__, __VA_ARGS__)
# define IS_LOG_PRIO(prio) (trace_log_mask & LOG_MASK(prio))
# define EMERG(msg, ...)						\
  if (IS_LOG_PRIO(LOG_EMERG)) TRACE_LOG("emerg:" msg, __VA_ARGS__)
# define ALERT(msg, ...)						\
  if (IS_LOG_PRIO(LOG_ALERT)) TRACE_LOG("alert:" msg, __VA_ARGS__)
# define CRIT(msg, ...)							\
  if (IS_LOG_PRIO(LOG_CRIT)) TRACE_LOG("crit:" msg, __VA_ARGS__)
# define ERR(msg, ...)							\
  if (IS_LOG_PRIO(LOG_ERR)) TRACE_LOG("err:" msg, __VA_ARGS__)
# define WARNING(msg, ...)						\
  if (IS_LOG_PRIO(LOG_WARNING)) TRACE_LOG("warning:" msg, __VA_ARGS__)
# define NOTICE(msg, ...)						\
  if (IS_LOG_PRIO(LOG_NOTICE)) TRACE_LOG("notice:" msg, __VA_ARGS__)
# define INFO(msg, ...)							\
  if (IS_LOG_PRIO(LOG_INFO)) TRACE_LOG("info:" msg, __VA_ARGS__)
# define DEBUG(msg, ...)						\
  if (IS_LOG_PRIO(LOG_DEBUG)) TRACE_LOG("debug:" msg, __VA_ARGS__)
#else
# define TRACE_PSTR(str)
# define TRACE(expr)
# define TRACE_LOG(msg, ...)
# define EMERG(msg, ...)
# define ALERT(msg, ...)
# define CRIT(msg, ...)
# define ERR(msg, ...)
# define WARNING(msg, ...)
# define NOTICE(msg, ...)
# define INFO(msg, ...)
# define DEBUG(msg, ...)
#endif

#endif
