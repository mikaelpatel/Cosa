/**
 * @file CosaMPEsender.ino
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
 * Demonstraction of Cosa MPE (Manchester Phase Encoding); 
 * Connect RF433 sender module to Board::D9.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/MPE.hh"
#include "Cosa/Watchdog.hh"

MPE::Transmitter tx(Board::D9);

void setup()
{
  Watchdog::begin();
  tx.begin();
}

void loop()
{
  static uint8_t msg[16] = { 0 };
  tx.send(&msg, sizeof(msg));
  msg[0] += 1;
  Watchdog::delay(16);
}

