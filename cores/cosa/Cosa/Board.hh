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
 * Cosa Board pin symbol definitions mapping to difference 
 * board/processor types.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_HH__
#define __COSA_BOARD_HH__

#include "Cosa/Types.h"
#if defined(__AVR_ATmega328P__)
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
#define __ARDUINO_TINY__ 
#define __ARDUINO_TINYX5__
#elif defined(__AVR_ATtiny24__)			\
  || defined(__AVR_ATtiny44__)			\
  || defined(__AVR_ATtiny84__)
#include "Cosa/Board/TinyX4.hh"
#define __ARDUINO_TINY__ 
#define __ARDUINO_TINYX4__
#else
#error "Cosa/Board.hh: board not supported"
#endif

#endif

