/**
 * @file CosaDS1990A.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Simple iButton reader and authorization checker.
 *
 * @section Extensions
 * Move key table to EEPROM. Allow update with a master key.
 * Presenting the master key will add a new key/remove an old key.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#include "Cosa/OWI.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Table with valid keys (64 bit 1-Wire identity, 8 bytes per entry)
uint8_t KEY[] __PROGMEM = {
  0x01, 0x23, 0x81, 0xa3, 0x09, 0x00, 0x00, 0x7b,
  0x01, 0x29, 0x01, 0x27, 0x09, 0x00, 0x00, 0xa8,
  0x01, 0x26, 0xd9, 0x3e, 0x09, 0x00, 0x00, 0x47
};

// One-wire pin (D1 on ATtiny, D7 on others)
#if defined(BOARD_ATTINY)
OWI owi(Board::D1);
OutputPin redLed(Board::D2);
OutputPin greenLed(Board::D4);
#else
OWI owi(Board::D7);
OutputPin ledPin(Board::LED);
#define redLed ledPin
#define greenLed ledPin
#endif

void setup()
{
  // Initiate UART as trace output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS1990A: started"));

  // Initiate the watchdog for low-power sleep mode
  Watchdog::begin();
}

void loop()
{
  // Take a nap
  SLEEP(1);

  // Check if a key was connected
  OWI::Driver dev(&owi);
  if (!dev.read_rom()) return;

  // Check if it is an authorized key. Turn on led for 5 seconds
  uint8_t* rom = dev.get_rom();
  for (uint8_t i = 0; i < sizeof(KEY); i += OWI::ROM_MAX) {
    if (!memcmp_P(rom, &KEY[i], OWI::ROM_MAX)) {
      trace << dev << PSTR(":AUTHORIZED KEY") << endl;
      greenLed.on();
      SLEEP(5);
      greenLed.off();
      return;
    }
  }

  // Otherwise it is an illegal key. Flash led for 5 seconds
  trace << dev << PSTR(":ILLEGAL KEY") << endl;
  for (uint8_t i = 0; i < 10; i++) {
    redLed.on();
    MSLEEP(250);
    redLed.off();
    MSLEEP(250);
  }
}

