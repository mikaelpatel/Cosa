/**
 * @file CosaPCD8544.ino
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
 * Demonstration of the PCD8544 device driver with mapping to
 * IOStream::Device.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/PCD8544.hh"
#include "Cosa/IOStream/Driver/UART.hh"

PCD8544 lcd;
#undef putchar

void setup()
{
  lcd.begin();
  lcd.set_mode(0);
  trace.begin(&lcd, PSTR("\fCosaPCD8544: started"));
  lcd.set_mode(1);
  trace  << PSTR("ABC") << endl;
  trace << bin << 0x55 << endl;
  trace << oct << 0x1234 << endl;
  trace << hex << 0x1234;
}

void loop()
{
}
