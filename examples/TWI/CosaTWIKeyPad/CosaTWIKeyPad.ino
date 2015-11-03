/**
 * @file CosaTWIKeyPad.ino
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
 * Demonstration of a simple 3X4 keypad decoder using the PCF8574
 * Remote 8-bit I/O expander.
 *
 * @section Circuit
 * This demo was constructed using a MJKDZ LCD TWI adapter and a 4X4
 * soft keypad. Only 3X4 keys are used as the adapter back-light pin
 * is difficult to use for sampling. Below is to wiring from adapter
 * to keypad pins (left to right).
 * LCD Adapter => KEYPAD
 * @code
 *   D7 => P0
 *   D6 => P1
 *   P5 => P2
 *   P4 => P3
 *   E  => P4
 *   RW => P5
 *   RS => P6
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <PCF8574.h>

#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"

class KeyPad3X4 : private PCF8574 {
public:
  /**
   * Construct keypad interface using the two-wire parallel port
   * adapter.
   * @param[in] subaddr address of unit (0..7, default 0).
   */
  KeyPad3X4(uint8_t subaddr = 0x00) : PCF8574(subaddr) {}

  /**
   * Initiate keypad interface. Return true if successful otherwise false.
   * @return bool.
   */
  bool begin();

  /**
   * Sample keypad for key pressed. Return key character (0..9) or
   * character (star or hash) if successful otherwise negative error
   * code(-1).
   * @return value or negative error code(-1).
   */
  int sample();
};

bool
KeyPad3X4::begin()
{
  // Set the data direction; Use D4-D7 for input (key pad rows)
  if (!data_direction(0xf0)) return (false);
  // Write zero to detect key pressed
  write(0);
  return (true);
}

int
KeyPad3X4::sample()
{
  // Sample the port and quick check if no key is pressed
  uint8_t value = read();
  int res = -1;
  if (value == 0xf0) return (res);
  // Key was pressed; check which row and then which column
  write(0xff);
  for (uint8_t row = 0; row < 4; row++) {
    // Write zero to detect which row the keys on
    write(row, 0);
    value = read();
    // On next row?
    if (value == 0xf0) continue;
    // Check which column
    for (uint8_t column = 0; column < 3; column++) {
      // Not found yet
      if (value & _BV(4 + column)) continue;
      // Found; adjust row-column to character
      if (row != 0) {
	res = '1' + column + (3 * (3 - row));
      } else {
	if (column == 0) res = '*';
	else if (column == 1) res = '0';
	else res = '#';
      }
      break;
    }
    break;
  }
  // And return result
  write(0x00);
  return (res);
}

KeyPad3X4 keypad;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTWIKeyPad: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(KeyPad3X4));
  TRACE(sizeof(TWI));

  // Start the watchdog ticks and keypad
  Watchdog::begin();
  keypad.begin();
}

void loop()
{
  // Periodically sample the keypad for key pressed
  Watchdog::delay(100);
  int key = keypad.sample();
  if (key == -1) return;

  // Print the key as character code and debounce by waiting
  trace << PSTR("key = \'") << (char) key << '\'' << endl;
  Watchdog::delay(500);
}

