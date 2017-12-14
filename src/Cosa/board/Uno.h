/**
 * @file Cosa/Board/Arduino/Uno.map
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include <avr/pgmspace.h>
#include "Uno.hh"

const Board::DigitalPin digital_pin_map[] PROGMEM = {
  Board::D0,
  Board::D1,
  Board::D2,
  Board::D3,
  Board::D4,
  Board::D5,
  Board::D6,
  Board::D7,
  Board::D8,
  Board::D9,
  Board::D10,
  Board::D11,
  Board::D12,
  Board::D13,
  Board::D14,
  Board::D15,
  Board::D16,
  Board::D17,
  Board::D18,
  Board::D19
};

const Board::AnalogPin analog_pin_map[] PROGMEM = {
  Board::A0,
  Board::A1,
  Board::A2,
  Board::A3,
  Board::A4,
  Board::A5
};

const Board::PWMPin pwm_pin_map[] PROGMEM = {
  Board::PWM0,
  Board::PWM1,
  Board::PWM2,
  Board::PWM3,
  Board::PWM4,
  Board::PWM5
};