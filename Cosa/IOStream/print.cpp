/**
 * @file Cosa/IOStream/print.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Print of memory blocks with address and bytes.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream.h"

void 
IOStream::print(void *ptr, size_t size, uint8_t base)
{
  uint8_t* p = (uint8_t*) ptr;
  uint8_t n_max = (base < 8 ? 8 : 16);
  unsigned int v_adj = (base == 10 ? 0 : (base == 8 ? 01000 : 0x100));
  uint8_t adj = (v_adj != 0);
  uint8_t n = 0;
  for (int i = 0; i < size; i++) {
    if (n == 0) {
      print(p);
      print_P(PSTR(": "));
    }
    char buf[sizeof(int) * CHARBITS + 1];
    unsigned int v = (*p++) + v_adj;
    print(utoa(v, buf, base) + adj);
    if (++n < n_max) {
      print_P(PSTR(" "));
    }
    else {
      println();
      n = 0;
    }
  }
  if (n != 0) println();
}
