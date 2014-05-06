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
 * board/processor types. Board symbols are set by the build,
 * e.g. -DARDUINO_UNO, and defined in the boards.txt configuration
 * file. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_BOARD_HH
#define COSA_BOARD_HH

#include "Cosa/Types.h"

#if defined(ARDUINO_UNO)			\
  || defined(ARDUINO_DUEMILANOVE)		\
  || defined(ARDUINO_NANO)			\
  || defined(ARDUINO_PRO_MINI)			\
  || defined(BREADBOARD_ATMEGA328P)		\
  || defined(LILYPAD_ARDUINO)			\
  || defined(MICRODUINO_CORE)
#include "Cosa/Board/Arduino/ATmega328P.hh"
#define BOARD_ATMEGA328P

#elif defined(ARDUINO_LEONARDO)			\
  || defined(ARDUINO_MICRO)			\
  || defined(ARDUINO_PRO_MICRO)			\
  || defined(LILYPAD_ARDUINO_USB)		\
  || defined(MICRODUINO_CORE32U4)
#include "Cosa/Board/Arduino/ATmega32U4.hh"
#define BOARD_ATMEGA32U4

#elif defined(ARDUINO_MEGA2560)
#include "Cosa/Board/Arduino/ATmega2560.hh"
#define BOARD_ATMEGA2560

#elif defined(BREADBOARD_ATTINYX4)
#include "Cosa/Board/Arduino/ATtinyX4.hh"
#define BOARD_ATTINYX4
#define BOARD_ATTINY

#elif defined(BREADBOARD_ATTINYX5)
#include "Cosa/Board/Arduino/ATtinyX5.hh"
#define BOARD_ATTINYX5
#define BOARD_ATTINY

#elif defined(BREADBOARD_ATTINYX61)
#include "Cosa/Board/Arduino/ATtinyX61.hh"
#define BOARD_ATTINYX61
#define BOARD_ATTINY

#elif defined(BREADBOARD_ATMEGA1284)		\
  ||  defined(MICRODUINO_CORE_PLUS)
#include "Cosa/Board/Arduino/ATmega1284P.hh"
#define BOARD_ATMEGA1248P

#elif defined(PINOCCIO_SCOUT)
#include "Cosa/Board/Pinoccio/ATmega256RFR2.hh"
#define BOARD_ATMEGA256RFR2

#elif defined(TEENSY_2_0)
#include "Cosa/Board/Teensy/ATmega32U4.hh"
#define BOARD_ATMEGA32U4

#else
#error "Cosa/Board.hh: board not supported"
#endif

#endif

