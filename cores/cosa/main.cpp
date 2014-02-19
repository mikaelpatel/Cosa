/**
 * @file main.cpp
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
 * Cosa Arduino main program. Calls sketch functions; setup() and
 * loop() and handles the iteration. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/LED.hh"

#if defined(USBCON)
#include "Cosa/USB/Platform.h"
#endif

/**
 * The init function; minimum setup of hardware after the bootloader.
 * This function may be overridden.
 */
void init() __attribute__((weak));
void init()
{
  // Set analog converter prescale factor and but do not enable conversion
#if F_CPU >= 16000000L
  ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
#elif F_CPU >= 8000000L
  ADCSRA |= (_BV(ADPS2) | _BV(ADPS1)             );
#else
  ADCSRA |= (             _BV(ADPS1) | _BV(ADPS0));
#endif
  
  // Disable analog comparator 
  ACSR = _BV(ACD);

  // The bootloader connects pins 0 and 1 to the USART; disconnect them
  // here so they can be used as normal digital IO.
#if defined(UCSRB)
  UCSRB = 0;
#elif defined(UCSR0B)
  UCSR0B = 0;
#endif

  // Attach the USB module and possible CDC/HID
#if defined(USBCON)
  USBDevice.attach();
#endif

  // Allow interrupts from here on
  sei();
}

/**
 * The default setup function; initiate the watchdog. This function may be
 * overridden.
 */
void setup() __attribute__((weak));
#if defined(COSA_DEFAULT_SETUP)
void setup()
{
  // Start the watchdog ticks and push time events
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Start the built-in LED in alert mode
  static LED builtin;
  builtin.alert_mode();
}
#endif

/**
 * The default loop function; event dispatcher. This function may be
 * overridden.
 */
void loop() __attribute__((weak));
#if defined(COSA_DEFAULT_LOOP)
void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
#endif

/**
 * The main function. This function may be overridden.
 */
int main(void) __attribute__((weak));
int main(void)
{
  init();
  setup();
  while (1) loop();
  return (0);
}

/**
 * Support for local static variables
 */
namespace __cxxabiv1 
{
  typedef int __guard;
  extern "C" int __cxa_guard_acquire (__guard *g) { return (1); }
  extern "C" void __cxa_guard_release (__guard *g) {}
  extern "C" void __cxa_guard_abort (__guard *) {}
  extern "C" void __cxa_pure_virtual(void) {}; 
}
