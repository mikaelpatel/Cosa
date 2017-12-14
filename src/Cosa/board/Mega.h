/**
 * @file Cosa/Board/Arduino/Mega.map
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
        Board::D19,
        Board::D20,
        Board::D21,
        Board::D22,
        Board::D23,
        Board::D24,
        Board::D25,
        Board::D26,
        Board::D27,
        Board::D28,
        Board::D29,
        Board::D30,
        Board::D31,
        Board::D32,
        Board::D33,
        Board::D34,
        Board::D35,
        Board::D36,
        Board::D37,
        Board::D38,
        Board::D39,
        Board::D40,
        Board::D41,
        Board::D42,
        Board::D43,
        Board::D44,
        Board::D45,
        Board::D46,
        Board::D47,
        Board::D48,
        Board::D49,
        Board::D50,
        Board::D51,
        Board::D52,
        Board::D53,
        Board::D54,
        Board::D55,
        Board::D56,
        Board::D57,
        Board::D58,
        Board::D59,
        Board::D60,
        Board::D61,
        Board::D62,
        Board::D63,
        Board::D64,
        Board::D65,
        Board::D66,
        Board::D67,
        Board::D68,
        Board::D69
};

const Board::AnalogPin analog_pin_map[]
PROGMEM = {
        Board::A0,
        Board::A1,
        Board::A2,
        Board::A3,
        Board::A4,
        Board::A5,
        Board::A6,
        Board::A7,
        Board::A8,
        Board::A9,
        Board::A10,
        Board::A11,
        Board::A12,
        Board::A13,
        Board::A14,
        Board::A15
};

const Board::PWMPin pwm_pin_map[] PROGMEM = {
        Board::PWM0,
        Board::PWM1,
        Board::PWM2,
        Board::PWM3,
        Board::PWM4,
        Board::PWM5,
        Board::PWM6,
        Board::PWM7,
        Board::PWM8,
        Board::PWM9,
        Board::PWM10,
        Board::PWM11
};