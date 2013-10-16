/**
 * @file CosaLCDslave.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Cosa demonstration of a Virtual LCD (VLCD).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/VLCD.hh"

// Use a 4-bit parallel port for the HD44780 LCD (16X2 default)
HD44780::Port4b port;
HD44780 lcd(&port);

// And use the LCD for the implementation of the Virtual LCD slave
VLCD::Slave vlcd(&lcd);

void setup()
{
  Watchdog::begin();
  lcd.begin();
  vlcd.begin();
}

