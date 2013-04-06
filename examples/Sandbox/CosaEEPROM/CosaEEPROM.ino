/**
 * @file CosaEEPROM.ino
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
 * Demo of the default EEPROM device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/EEPROM.hh"

EEPROM eeprom;
static const int DATA_MAX = 8;
long data[DATA_MAX] EEMEM;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaEEPROM: started"));
  TRACE(free_memory());
  Watchdog::begin();
  
  for (uint8_t i = 0; i < DATA_MAX; i++) {
    eeprom.write(&data[i], (long) i);
  }
}

void loop()
{
  static int i = 0;
  long x;
  TRACE(eeprom.read(&x, &data[i]));
  trace << i << ':' << x << endl;
  x += 5;
  TRACE(eeprom.write(&data[i], x));
  i += 1;
  if (i == membersof(data)) i = 0;
  SLEEP(2);
}
