/**
 * @file Cosa/Power.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_POWER_HH__
#define __COSA_POWER_HH__

#include "Cosa/Types.h"
#include "Cosa/Board.hh"

#include <avr/power.h>
#include <avr/sleep.h>

/**
 * Power Management and Sleep modes.
 */
class Power {
  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  Power() {}

public:
  /**
   * Put the processor in the given sleep mode and wait for 
   * an interrupt to wake up.
   * @param[in] mode sleep mode, see <avr/sleep.h>
   */
  static void sleep(uint8_t mode = SLEEP_MODE_IDLE);

  static void adc_enable() { power_adc_enable(); }
  static void adc_disable() { power_adc_disable(); }

  static void timer0_enable() { power_timer0_enable(); }
  static void timer0_disable() { power_timer0_disable(); }

  static void timer1_enable() { power_timer1_enable(); }
  static void timer1_disable() { power_timer1_disable(); }

#if !defined(__ARDUINO_TINYX5__)

  static void spi_enable() { power_spi_enable(); }
  static void spi_disable() { power_spi_disable(); }

  static void twi_enable() { power_twi_enable(); }
  static void twi_disable() { power_twi_disable(); }

  static void timer2_enable() { power_timer2_enable(); }
  static void timer2_disable() { power_timer2_disable(); }

  static void usart0_enable() { power_usart0_enable(); }
  static void usart0_disable() { power_usart0_disable(); }

#if defined(__ARDUINO_MIGHTY__)

  static void usart1_enable() { power_usart1_enable(); }
  static void usart1_disable() { power_usart1_disable(); }

  static void timer3_enable() { power_timer3_enable(); }
  static void timer3_disable() { power_timer3_disable(); }

#elif defined(__ARDUINO_MEGA__)
  
  static void usart1_enable() { power_usart1_enable(); }
  static void usart1_disable() { power_usart1_disable(); }

  static void usart2_enable() { power_usart2_enable(); }
  static void usart2_disable() { power_usart2_disable(); }

  static void usart3_enable() { power_usart3_enable(); }
  static void usart3_disable() { power_usart3_disable(); }

  static void timer3_enable() { power_timer3_enable(); }
  static void timer3_disable() { power_timer3_disable(); }

  static void timer4_enable() { power_timer4_enable(); }
  static void timer4_disable() { power_timer4_disable(); }

  static void timer5_enable() { power_timer5_enable(); }
  static void timer5_disable() { power_timer5_disable(); }
#endif

#elif defined(__ARDUINO_TINYX5__)
  static void usi_enable() { power_usi_enable(); }
  static void usi_disable() { power_usi_disable(); }
#endif

  static void all_enable() { power_all_enable(); }
  static void all_disable() { power_all_disable(); }
};

#endif
