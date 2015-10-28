/**
 * @file CosaIR.ino
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
 * Cosa IR receiver for LG remote using a TSOP4838 for decoding
 * IR transmission. Uses the Watchdog to monitor the decoding.
 *
 * @section Circuit
 * PCD8544 is a low voltage device (3V3) and signals require
 * level shifter (74HC4050 or 10K resistor).
 * @code
 *                       TSOP4838/ir
 *                       +------------+
 * (D2)----------------1-|OUT         |
 * (GND)---------------2-|GND    ( )  |
 * (VCC)---------------3-|VCC         |
 *                       +------------+
 *
 *                       PCD8544/lcd
 *                       +------------+
 * (RST)---| > |-------1-|RST         |
 * (D9/D3)-| > |-------2-|CE          |
 * (D8/D2)-| > |-------3-|DC          |
 * (D6/D0)-| > |-------4-|DIN         |
 * (D7/D1)-| > |-------5-|CLK         |
 * (3V3)---------------6-|VCC         |
 * (GND)---|220|-------7-|LED         |
 * (GND)---------------8-|GND         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <IR.h>

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

#if defined(BOARD_ATTINY)
#define USE_UART
#endif

/**
 * Use UART or LCD/PCD8544 for output.
 */
#if defined(USE_UART)
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"
#else
#include <Canvas.h>
#include <PCD8544.h>
#endif

/**
 * Key mapping for LG AKB72913104. Store in program memory.
 */
static const IR::Receiver::keymap_t LG_keymap[] __PROGMEM = {
  { 0x08f7, '0'  },
  { 0x8877, '1'  },
  { 0x48b7, '2'  },
  { 0xc837, '3'  },
  { 0x28d7, '4'  },
  { 0xa857, '5'  },
  { 0x6897, '6'  },
  { 0xe817, '7'  },
  { 0x18e7, '8'  },
  { 0x9867, '9'  },
  { 0x40bf, '+'  },
  { 0xc03f, '-'  },
  { 0x00ff, '<'  },
  { 0x807f, '>'  },
  { 0x22dd, '\n' },
  { 0xd02f, '\f' },
  { 0x10ef, '~'  },
  { 0x906f, '#'  },
  { 0x58a7, '\b' }
};

/**
 * Samples are collected when using an UART.
 */
#if defined(USE_UART)
uint16_t sample[40];
#else
// Select PCD8544 IO Adapter; Serial Output Pins or SPI
// LCD::Serial3W port;
LCD::SPI3W port;
PCD8544 lcd(&port);
#define sample NULL
#endif

// Use the watchdog job scheduler
Watchdog::Scheduler scheduler;

/**
 * IR receiver for LG AKB72913104, 36 samples, threshold (1000 + 1000/2).
 */
IR::Receiver receiver(Board::EXT0, 36, 1500, &scheduler,
		      LG_keymap, membersof(LG_keymap),
		      sample);

void setup()
{
  // Start trace output stream on the serial port
#ifdef USE_UART
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaIR: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Link));
#if !defined(BOARD_ATTINY)
  TRACE(sizeof(ExternalInterruptPin));
#endif
  TRACE(sizeof(IR::Receiver));
#else
  lcd.begin();
  trace.begin(&lcd, PSTR("\fOFF"));
#endif

  // Use the real-time timer for time measurement
  RTT::begin();

  // Use the watchdog
  Watchdog::begin();

  // Reset the receiver to start up clean
  receiver.reset();
}

void loop()
{
  // Wait for an event from the IR receiver
  Event event;
  Event::queue.await(&event);
  uint8_t type = event.type();

  // Check if a new reading from the IR receiver was completed
  if (type == Event::READ_COMPLETED_TYPE) {
    static uint8_t is_on = 0;
    uint16_t code = event.value();
    char key = receiver.lookup(code);
    trace << receiver;
    // Check special keys; first on/off
    if (key == '~') {
      trace << '\f' << (is_on ? PSTR("OFF") : PSTR("ON")) << endl;
      is_on = !is_on;
    }
    else if (is_on) {
      // Backspace
      if (key == '\b')
	trace << key << ' ' << key;
      // Form-feed
      else if (key == '\f')
	trace << key << PSTR("CLEAR") << endl;
      // New-line
      else if (key == '\n')
	trace << '.' << endl;
      // And other keys
      else if (key != -1)
	trace << key;
    }
  }

  // Reset for the next sequence
  receiver.reset();
}
