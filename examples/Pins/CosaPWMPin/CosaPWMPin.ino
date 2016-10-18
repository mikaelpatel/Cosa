/**
 * @file CosaPWMPin.ino
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
 * Cosa demonstration of PWM pins (PWM0/D3).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/PWMPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

PWMPin ledPin(Board::PWM0);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPWMPin: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Pin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(PWMPin));

  // Start PWM pin
  ledPin.begin();

  // Start the watchdog ticks counter
  Watchdog::begin();
}

void loop()
{
  static int duty = 0;
  static bool up = true;

  // Change the duty
  ledPin.set(duty);
  Watchdog::delay(32);

  // Increment or decrement depending of direction
  if (up) {
    duty += 8;
    if (duty > 255) {
      up = false;
      duty = 255;
    }
  }
  else {
    duty -= 8;
    if (duty < 0) {
      duty = 0;
      up = true;
    }
  }
}
