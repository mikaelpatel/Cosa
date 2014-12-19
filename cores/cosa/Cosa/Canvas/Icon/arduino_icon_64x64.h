/**
 * @file Cosa/Canvas/Icon/arduino_icon_64x64.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#ifndef COSA_CANVAS_ICON_ARDUINO_64X64_H
#define COSA_CANVAS_ICON_ARDUINO_64X64_H

/**
 * Arduino icon 64x64.
 *
 * @section Acknowledgements
 * Originates from the GLCD library and adapted for Cosa Canvas.
 * The GLCD library was created by Michael Margolis and improved 
 * by Bill Perry.
 */
const uint8_t arduino_icon_64x64[] __PROGMEM = {
#include "Data/Arduino64x64.h"
};

#endif
