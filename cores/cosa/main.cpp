/**
 * @file main.cpp
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
 * Cosa Arduino main program. Calls sketch functions; setup() and
 * loop() and handles the iteration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <avr/interrupt.h>
#include <avr/io.h>

extern void setup(void);
extern void loop(void);

void init()
{
  // Set ADC prescale factor to 128 and enable conversion
  // 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
#if F_CPU >= 16000000L
  ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0) | _BV(ADEN));
#elif F_CPU >= 8000000L
  ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADEN));
#else
  ADCSRA |= (_BV(ADPS1) | _BV(ADPS0) | _BV(ADEN));
#endif
  
  // The bootloader connects pins 0 and 1 to the USART; disconnect them
  // here so they can be used as normal digital i/o.
#if defined(UCSRB)
  UCSRB = 0;
#elif defined(UCSR0B)
  UCSR0B = 0;
#endif

  // Allow interrupts
  sei();
}

int main(void)
{
  init();
  setup();
  while (1) loop();
  return (0);
}

