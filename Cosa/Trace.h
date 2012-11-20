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

extern Trace trace;

#ifndef NDEBUG

/**
 * Support macro for trace of a string in program memory.
 * @param[in] str string literal
 */
#define TRACE_PSTR(str)				\
  trace.print_P(PSTR(str))

/**
 * Support macro for trace of an expression. The expression
 * is used as a string and the value is evaluated.
 * @param[in] expr expression.
 */
# define TRACE(expr)				\
  do {						\
    TRACE_PSTR(#expr " = ");			\
    trace.print(expr);				\
    trace.println();				\
  } while (0)

/**
 * Support macro for trace of a log message with line number and
 * function name prefix.
 * @param[in] msg log message.
 */
# define TRACE_LOG(msg)				\
  do {						\
    trace.print(__LINE__);			\
    trace.print(':');				\
    trace.print(__func__);			\
    TRACE_PSTR(":" msg "\n");			\
  } while (0)
#endif

#endif
