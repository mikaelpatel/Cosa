/**
 * @file Cosa/Board.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "Cosa/Types.h"
#include "Board.h"

// Custom Boards
#if defined(COSA_CUSTOM_BOARD)
#include <Board.map>

// Arduino Boards
#elif defined(ARDUINO_DIECIMILA)
#include "Cosa/Board/Arduino/Diecimila.map"
#elif defined(ARDUINO_DUEMILANOVE)
#include "Cosa/Board/Arduino/Duemilanove.map"
#elif defined(ARDUINO_LEONARDO)
#include "Cosa/Board/Arduino/Leonardo.map"
#elif defined(ARDUINO_MEGA)
#include "Cosa/Board/Arduino/Mega.map"
#elif defined(ARDUINO_MICRO)
#include "Cosa/Board/Arduino/Micro.map"
#elif defined(ARDUINO_NANO)
#include "Cosa/Board/Arduino/Nano.map"
#elif defined(ARDUINO_PRO_MICRO)
#include "Cosa/Board/Arduino/Pro_Micro.map"
#elif defined(ARDUINO_PRO_MINI)
#include "Cosa/Board/Arduino/Pro_Mini.map"
#elif defined(ARDUINO_UNO)
#include "Cosa/Board/Arduino/Uno.map"

// Breadboards
#elif defined(BREADBOARD_ATTINYX4)
#include "Cosa/Board/Arduino/ATtinyX4.map"
#elif defined(BREADBOARD_ATTINYX5)
#include "Cosa/Board/Arduino/ATtinyX5.map"
#elif defined(BREADBOARD_ATTINYX61)
#include "Cosa/Board/Arduino/ATtinyX61.map"
#elif defined(BREADBOARD_ATMEGA328P)
#include "Cosa/Board/Arduino/ATmega328P.map"
#elif defined(BREADBOARD_ATMEGA1284P)
#include "Cosa/Board/Arduino/ATmega1284P.map"

// LilyPad Arduino Boards
#elif defined(LILYPAD_ARDUINO)
#include "Cosa/Board/Arduino/LilyPad.map"
#elif defined(LILYPAD_ARDUINO_USB)
#include "Cosa/Board/Arduino/LilyPad_USB.map"

#else
#error "Cosa/Board.hh: board not supported"
#endif


