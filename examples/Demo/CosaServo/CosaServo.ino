/**
 * @file CosaServo.ino
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
 * Demonstration of the Servo control class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Servo.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

Servo servo(0, Board::D9);
Servo door(1, Board::D8);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaServo: started"));
  Servo::begin();
  Watchdog::begin();
}

void loop()
{
  int degree;
  for (uint8_t inc = 5; inc < 20; inc += 5) {
    door.set_angle(0);
    for (degree = 0; degree < 180; degree += inc) {
      servo.set_angle(degree);
      Watchdog::delay(64);
    }
    door.set_angle(180);
    for (; degree > 0; degree -= inc) {
      servo.set_angle(degree);
      Watchdog::delay(64);
    }
  }
}
