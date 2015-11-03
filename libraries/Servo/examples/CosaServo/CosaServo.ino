/**
 * @file CosaServo.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Demonstration of the Servo control class.
 *
 * @section Circuit
 * @code
 *                       Servo#0/servo
 *                       +------------+
 * (D9)----------------1-|PULSE   I   |
 * (VCC)---------------2-|VCC   ==o== |
 * (GND)---------------3-|GND     I   |
 *                       +------------+
 *                       Servo#1/door
 *                       +------------+
 * (D8)----------------1-|PULSE   I   |
 * (VCC)---------------2-|VCC   ==o== |
 * (GND)---------------3-|GND     I   |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Servo.h>
#include "Cosa/Watchdog.hh"

Servo servo(0, Board::D9);
Servo door(1, Board::D8);

void setup()
{
  // Start watchdog for delay timing
  Watchdog::begin();

  // Start servo handling and set initial angle
  Servo::begin();
  door.angle(10);
  servo.angle(10);
}

void loop()
{
  static int degree = 10;
  static int inc = 45;

  // Step servo from 10 to 170 degrees by 45 degrees
  for (; degree < 170; degree += inc) {
    door.angle(degree);
    servo.angle(degree);
    delay(512);
  }
  if (degree > 170) degree -= inc;

  // Step servo from 170 to 10 degrees by -45 degrees
  for (; degree > 10; degree -= inc) {
    door.angle(degree);
    servo.angle(degree);
    delay(512);
  }
  if (degree < 10) degree += inc;
}
