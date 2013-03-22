/**
 * @file Cosa/Board.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * Cosa Board pin symbol definitions for the ATmega8, ATmega168
 * ATmega328P, ATmega1280 and ATmega2560 based Arduino boards;
 * Arduino Uno, Mini Pro, Nano, LilyPad and Mega 2560.
 * Cosa does not use pin numbers are Arduino. Instead strong
 * data type is used (enum types) for the specific pin classes;
 * e.g. InterruptPin, AnalogPin, PWMPin.
 *
 * @section Limitations
 * The pin numbers for ATmega8, ATmega168 and ATmega328P are mapped
 * as in Arduino but ATmega1280 and ATmega2560 are only symbolically
 * mapped, i.e. a pin number/digit will not work, symbols must be 
 * used, e.g., Board::D42.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_HH__
#define __COSA_BOARD_HH__

#include "Cosa/Types.h"
#if defined(__AVR_ATmega8__)			\
  || defined(__AVR_ATmega168__)			\
  || defined(__AVR_ATmega328P__)
#include "Cosa/Board/Standard.hh"
#define __ARDUINO_STANDARD__
#elif defined(__AVR_ATmega1284P__)
#include "Cosa/Board/Mighty.hh"
#define __ARDUINO_MIGHTY__
#elif defined(__AVR_ATmega1280__)		\
  || defined(__AVR_ATmega2560__)
#include "Cosa/Board/Mega.hh"
#define __ARDUINO_MEGA__
#elif defined(__AVR_ATtiny25__)			\
  || defined(__AVR_ATtiny45__)			\
  || defined(__AVR_ATtiny85__)
#include "Cosa/Board/TinyX5.hh"
#define __ARDUINO_TINYX5__
#else
#error "Cosa/Board.hh: board not supported"
#endif

#endif

