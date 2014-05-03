/**
 * @file Cosa/Board.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * @section Description
 * Cosa Board pin symbol definitions mapping to difference 
 * board/processor types.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_BOARD_HH
#define COSA_BOARD_HH

#include "Cosa/Types.h"

#if defined(__AVR_ATmega328P__)
#include "Cosa/Board/ATmega328P.hh"
#define BOARD_ATMEGA328P
#elif defined(__AVR_ATmega32U4__)
#include "Cosa/Board/ATmega32U4.hh"
#define BOARD_ATMEGA32U4
#elif defined(__AVR_ATmega1284P__)		\
  || defined(__AVR_ATmega644P__)
#include "Cosa/Board/ATmega1284P.hh"
#define BOARD_ATMEGA1248P
#elif defined(__AVR_ATmega1280__)		\
  || defined(__AVR_ATmega2560__)
#include "Cosa/Board/ATmega2560.hh"
#define BOARD_ATMEGA2560
#elif defined(__AVR_ATmega256RFR2__)
#include "Cosa/Board/ATmega256RFR2.hh"
#define BOARD_ATMEGA256RFR2
#elif defined(__AVR_ATtiny24__)			\
  || defined(__AVR_ATtiny44__)			\
  || defined(__AVR_ATtiny84__)
#include "Cosa/Board/ATtinyX4.hh"
#define BOARD_ATTINYX4
#define BOARD_ATTINY
#elif defined(__AVR_ATtiny25__)			\
  || defined(__AVR_ATtiny45__)			\
  || defined(__AVR_ATtiny85__)
#include "Cosa/Board/ATtinyX5.hh"
#define BOARD_ATTINYX5
#define BOARD_ATTINY
#elif defined(__AVR_ATtiny261__)		\
  || defined(__AVR_ATtiny461__)			\
  || defined(__AVR_ATtiny861__)
#include "Cosa/Board/ATtinyX61.hh"
#define BOARD_ATTINYX61
#define BOARD_ATTINY
#else
#error "Cosa/Board.hh: board not supported"
#endif

#endif

