/**
 * @file CosaTSOP4838.ino
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
 * Cosa IR receiver for LG remote using a TSOP4838 for decoding
 * IR transmission. Uses the Watchdog to monitor the decoding.
 *
 * @section Circuit
 * Connect data output from IR receiver circuit to pin D2 (EXT0).
 * Don't forget VCC and GND for the IR receiver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IR.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

#if defined(__ARDUINO_TINY__)
#define USE_UART
#endif

/**
 * Use UART or LCD/PCD8544 for output. 
 */
#if defined(USE_UART)
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"
#else
#include "Cosa/LCD/Driver/PCD8544.hh"
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
PCD8544 lcd;
#define sample 0
#endif

/**
 * IR receiver for LG AKB72913104, 36 samples, threshold (1000 + 1000/2).
 */
IR::Receiver receiver(Board::EXT0, 
		      36, 1500, 
		      LG_keymap, membersof(LG_keymap),
		      sample);

void setup()
{
  // Start trace output stream on the serial port
#ifdef USE_UART
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaIRreceiver: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Link));
  TRACE(sizeof(ExternalInterruptPin));
  TRACE(sizeof(IR::Receiver));
#else
  lcd.begin();
  trace.begin(&lcd, PSTR("\fOFF"));
#endif

  // Use the real-time clock for time measurement
  RTC::begin();

  // Use the watchdog for timeouts
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Reset the receiver to start up clean
  receiver.reset();
}

void loop()
{
  // Wait for an event from the IR receiver
  Event event;
  Event::queue.await(&event);
  uint8_t type = event.get_type();

  // Check if a new reading from the IR receiver was completed
  if (type == Event::READ_COMPLETED_TYPE) {
    static uint8_t is_on = 0;
    uint16_t code = event.get_value();
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
