/**
 * @file
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
 * Hardware Serial (UART) support functions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_SERIAL_H__
#define __COSA_SERIAL_H__

#include "Types.h"
#include <HardwareSerial.h>

extern void __Serial_print(PGM_P s);

/**
 * Serial print of string in program memory
 * @param[in] x program memory string literal
 */
#define Serial_print(x) __Serial_print(PSTR(x))

/**
 * Serial trace of evaluation of expression
 * @param[in] expr expression to print and evaluate
 */
#define Serial_trace(expr)			\
  do {						\
    Serial_print(# expr " = ");			\
    Serial.println(expr);			\
  } while (0)					

#endif
