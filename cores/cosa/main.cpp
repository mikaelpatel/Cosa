/**
 * @file main.cpp
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
 * Cosa Arduino main program. Calls sketch functions; setup() and
 * loop() and handles the iteration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Cosa/CPU.hh"
#include "Cosa/Power.hh"

/**
 * The init function; minimum setup of hardware after the bootloader.
 * This function may be overridden.
 */
void init() __attribute__((weak));
void init()
{
  // Adjust frequency scaling on Teensy; default is no scaling on Cosa
#if defined(PJRC_TEENSY_2_0) || defined(PJRC_TEENSYPP_2_0)
  CPU::clock_prescale(0);
#endif

  // Set analog converter prescale factor and but do not enable conversion
#if F_CPU >= 16000000L
  ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
#elif F_CPU >= 8000000L
  ADCSRA |= (_BV(ADPS2) | _BV(ADPS1));
#else
  ADCSRA |= (_BV(ADPS1) | _BV(ADPS0));
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

  // Initiate USB when needed (when CDC is attached)
#if defined(USBCON)
  USBCON = 0;
  UDCON = 0;
  UDINT = 0;
  UDIEN = 0;
#endif

  // Power down all modules.
  Power::all_disable();

  // Allow the board to set ports in a safe state. Typically chip
  // select pins to board devices
  Board::init();

  // Allow interrupts from here on
  sei();
}

/**
 * Default setup function. This function may be overridden.
 */
void setup() __attribute__((weak));
void setup()
{
}

/**
 * Default loop function. This function may be overridden.
 */
void loop() __attribute__((weak));
void loop()
{
  exit(0);
}

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
 * The exit function. This function may be overridden.
 */
void exit(int status) __attribute__((weak));
void exit(int status)
{
  UNUSED(status);

  cli();

#if defined(USBCON)
  extern void USB_Keepalive(void);
  // Never returns
  USB_Keepalive();
#endif

  // Hang forever in sleep mode
  while (1)
    Power::sleep();
}

/**
 * Default delay function; busy-wait given number of milli-seconds.
 * @param[in] ms milli-seconds delay.
 */
static void default_delay(uint32_t ms)
{
  if (UNLIKELY(ms == 0)) return;
  while (ms--) DELAY(1000);
}

/**
 * Default sleep function; delay given number of seconds.
 * @param[in] s seconds delay.
 */
static void default_sleep(uint16_t s)
{
  delay(s * 1000L);
}

/**
 * Default yield function; enter sleep mode and wait for
 * any interrupt.
 */
static void default_yield()
{
  Power::sleep();
}

/* Default setting of multi-tasking functions */
void (*delay)(uint32_t ms) = default_delay;
void (*sleep)(uint16_t s) = default_sleep;
void (*yield)() = default_yield;

/**
 * Support for local static variables
 */
namespace __cxxabiv1
{
  typedef int __guard;

  extern "C" int __cxa_guard_acquire(__guard *g)
  {
    UNUSED(g);
    return (0);
  }

  extern "C" void __cxa_guard_release(__guard *g)
  {
    UNUSED(g);
  }

  extern "C" void __cxa_guard_abort(__guard *g)
  {
    UNUSED(g);
  }

  extern "C" void __cxa_pure_virtual(void)
  {
  }

  void *__dso_handle = NULL;

  extern "C" int __cxa_atexit(void (*destructor)(void*), void* arg, void* dso)
  {
    UNUSED(destructor);
    UNUSED(arg);
    UNUSED(dso);
    return 0;
  }

  extern "C" void __cxa_finalize(void* f)
  {
    UNUSED(f);
  }
}
