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

#include "Board.h"

// Arduino Boards
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#include "board/Mega.h"
#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#include "board/Uno.h"
#endif


