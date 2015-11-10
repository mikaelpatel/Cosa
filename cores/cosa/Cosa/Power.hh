/**
 * @file Cosa/Power.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_POWER_HH
#define COSA_POWER_HH

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include <avr/power.h>

/**
 * Power Management and Sleep modes.
 */
class Power {
public:
  /**
   * Set the default sleep mode: SLEEP_MODE_IDLE, SLEEP_MODE_ADC,
   * SLEEP_MODE_PWR_DOWN, SLEEP_MODE_PWR_SAVE, SLEEP_MODE_STANDBY,
   * and SLEEP_MODE_EXT_STANDBY.
   * @param[in] mode sleep mode, see <avr/sleep.h>
   * @return previous mode.
   */
  static uint8_t set(uint8_t mode)
    __attribute__((always_inline))
  {
    uint8_t res = s_mode;
    s_mode = mode;
    return (res);
  }

  /**
   * Put the processor in the given sleep mode and wait for
   * an interrupt to wake up.
   * @param[in] mode sleep mode, see <avr/sleep.h>
   */
  static void sleep(uint8_t mode = POWER_SLEEP_MODE);

  static void adc_enable()
    __attribute__((always_inline))
  {
    power_adc_enable();
    bit_set(ADCSRA, ADEN);
  }

  static void adc_disable()
    __attribute__((always_inline))
  {
    bit_clear(ADCSRA, ADEN);
    power_adc_disable();
  }

  static void timer0_enable()
    __attribute__((always_inline))
  {
    power_timer0_enable();
  }

  static void timer0_disable()
    __attribute__((always_inline))
  {
    power_timer0_disable();
  }

  static void timer1_enable()
    __attribute__((always_inline))
  {
    power_timer1_enable();
  }

  static void timer1_disable()
    __attribute__((always_inline))
  {
    power_timer1_disable();
  }

#if defined(power_timer2_enable)
  static void timer2_enable()
    __attribute__((always_inline))
  {
    power_timer2_enable();
  }

  static void timer2_disable()
    __attribute__((always_inline))
  {
    power_timer2_disable();
  }
#endif

#if defined(power_timer3_enable)
  static void timer3_enable()
    __attribute__((always_inline))
  {
    power_timer3_enable();
  }

  static void timer3_disable()
    __attribute__((always_inline))
  {
    power_timer3_disable();
  }
#endif

#if defined(power_timer4_enable)
  static void timer4_enable()
    __attribute__((always_inline))
  {
    power_timer4_enable();
  }

  static void timer4_disable()
    __attribute__((always_inline))
  {
    power_timer4_disable();
  }
#endif

#if defined(power_timer5_enable)
    __attribute__((always_inline))
  static void timer5_enable()
  {
    power_timer5_enable();
  }

  static void timer5_disable()
    __attribute__((always_inline))
  {
    power_timer5_disable();
  }
#endif

#if defined(power_usart0_enable)
  static void usart0_enable()
    __attribute__((always_inline))
  {
    power_usart0_enable();
  }

  static void usart0_disable()
    __attribute__((always_inline))
  {
    power_usart0_disable();
  }
#endif

#if defined(power_usart1_enable)
  static void usart1_enable()
    __attribute__((always_inline))
  {
    power_usart1_enable();
  }

  static void usart1_disable()
    __attribute__((always_inline))
  {
    power_usart1_disable();
  }
#endif

#if defined(power_usart2_enable)
  static void usart2_enable()
    __attribute__((always_inline))
  {
    power_usart2_enable();
  }

  static void usart2_disable()
    __attribute__((always_inline))
  {
    power_usart2_disable();
  }
#endif

#if defined(power_usart3_enable)
  static void usart3_enable()
    __attribute__((always_inline))
  {
    power_usart3_enable();
  }

  static void usart3_disable()
    __attribute__((always_inline))
  {
    power_usart3_disable();
  }
#endif

#if defined(power_usb_enable)
  static void usb_enable()
    __attribute__((always_inline))
  {
    power_usb_enable();
  }

  static void usb_disable()
    __attribute__((always_inline))
  {
    power_usb_disable();
  }
#endif

#if defined(power_usi_enable)
  static void usi_enable()
    __attribute__((always_inline))
  {
    power_usi_enable();
  }

  static void usi_disable()
    __attribute__((always_inline))
  {
    power_usi_disable();
  }
#endif

#if defined(power_spi_enable)
  static void spi_enable()
    __attribute__((always_inline))
  {
    power_spi_enable();
  }

  static void spi_disable()
    __attribute__((always_inline))
  {
    power_spi_disable();
  }
#endif

#if defined(power_twi_enable)
  static void twi_enable()
    __attribute__((always_inline))
  {
    power_twi_enable();
  }

  static void twi_disable()
    __attribute__((always_inline))
  {
    power_twi_disable();
  }
#endif

  static void all_enable()
    __attribute__((always_inline))
  {
    power_all_enable();
    bit_set(ADCSRA, ADEN);
  }

  static void all_disable()
    __attribute__((always_inline))
  {
    bit_clear(ADCSRA, ADEN);
    power_all_disable();
  }

private:
  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  Power() {}

  /** Use Power sleep mode flag. */
  static const uint8_t POWER_SLEEP_MODE = 0xff;

  /** Current sleep mode. */
  static uint8_t s_mode;
};

#endif
