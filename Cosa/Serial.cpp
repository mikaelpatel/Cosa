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

#include "Serial.h"

void __Serial_print(PGM_P s)
{
  /*
   * Workaround for strings in program memory
   * http://jeelabs.org/2011/05/23/saving-ram-space/
   */
  char c;
  while ((c = pgm_read_byte(s++)) != 0)
    Serial.print(c);
}

