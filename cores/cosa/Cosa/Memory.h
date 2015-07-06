/**
 * @file Cosa/Memory.h
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_MEMORY_H
#define COSA_MEMORY_H

/**
 * Return amount of free memory.
 * @return number of bytes.
 */
inline int free_memory() __attribute__((always_inline));
inline int
free_memory()
{
  extern int __heap_start, *__brkval;
  int v;
  return ((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
}

#endif
