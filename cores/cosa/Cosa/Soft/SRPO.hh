/**
 * @file Cosa/Soft/SRPO.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_SOFT_SRPO_HH
#define COSA_SOFT_SRPO_HH

#include "Cosa/OutputPin.hh"

namespace Soft {

/**
 * Soft N-Shift Register Parallel Output, 2-pin. The shift registers
 * (74HC164/74HC595) may be cascaded for N*8-bit parallel output port
 * (see circuit below). The pins are numbered from the first connect
 * shift register (Q0..Q7) and updwards in the chain (Q8..Q15) and so
 * on.
 *
 * @section Circuit
 * @code
 *                         74HC164    (VCC)
 *                       +----U----+    |
 * (D3/SDA)----------+-1-|DSA   VCC|-14-+
 *                   +-2-|DSB    Q7|-13------------(Q7)--+
 * (Q0)----------------3-|Q0     Q6|-12------------(Q6)  |
 * (Q1)----------------4-|Q1     Q5|-11------------(Q5)  |
 * (Q2)----------------5-|Q2     Q4|-10------------(Q4)  |
 * (Q3)----------------6-|Q3    /MR|--9-----------(VCC)  |
 *                   +-7-|GND    CP|--8--------(SCL/D4)--)-+
 *                   |   +---------+                     | |
 *                   |      0.1uF                        | |
 *                 (GND)-----||-------(VCC)              | |
 *                                                       | |
 *                   +-----------------------------------+ |
 *                   |                                     |
 *                   |     74HC164    (VCC)                |
 *                   |   +----U----+    |                  |
 *                   +-1-|DSA   VCC|-14-+                  |
 *                   +-2-|DSB    Q7|-13-----------(Q15)--+ |
 * (Q8)----------------3-|Q0     Q6|-12-----------(Q14)  | |
 * (Q9)----------------4-|Q1     Q5|-11-----------(Q13)  | |
 * (Q10)---------------5-|Q2     Q4|-10-----------(Q12)  | |
 * (Q11)---------------6-|Q3    /MR|--9-----------(VCC)  | |
 *                   +-7-|GND    CP|--8------------------)-+
 *                   |   +---------+                     | |
 *                   |      0.1uF                        | |
 *                 (GND)-----||-------(VCC)              v v
 * @endcode
 *
 * @param[in] N number of shift registers (N * 8 output pins).
 */
template<uint8_t N>
class SRPO {
public:
  /** Number of pins for N ports */
  static const uint8_t PINS = N * CHARBITS;

  /**
   * Construct N-shift register connected to given pins.
   * @param[in] sda serial output data (Default D3).
   * @param[in] scl serial clock (Default D4).
   */
  SRPO(Board::DigitalPin sda = Board::D3,
       Board::DigitalPin scl = Board::D4) :
    m_sda(sda),
    m_scl(scl)
  {
    clear();
    update();
  }

  /**
   * Return true(1) if the given pin in shadow register is set,
   * otherwise false(0).
   * @param[in] pin pin number.
   * @return bool.
   */
  bool is_set(uint8_t pin)
    __attribute__((always_inline))
  {
    uint8_t ix = (pin >> 3);
    return ((m_port[ix] & _BV(pin & 0x7)) != 0);
  }

  /**
   * Return true(1) if the given pin in shadow register is set,
   * otherwise false(0).
   * @param[in] pin pin number.
   * @return bool.
   */
  void is_clear(uint8_t pin)
    __attribute__((always_inline))
  {
    uint8_t ix = (pin >> 3);
    return ((m_port[ix] & _BV(pin & 0x7)) == 0);
  }

  /**
   * Set given pin in shadow register. Call update() to write to shift
   * register.
   * @param[in] pin pin number.
   */
  void set(uint8_t pin)
    __attribute__((always_inline))
  {
    uint8_t ix = (pin >> 3);
    m_port[ix] |= _BV(pin & 0x7);
  }

  /**
   * Clear given pin in shadow register. Call update() to write to shift
   * register.
   * @param[in] pin pin number.
   */
  void clear(uint8_t pin)
    __attribute__((always_inline))
  {
    uint8_t ix = (pin >> 3);
    m_port[ix] &= ~_BV(pin & 0x7);
  }

  /**
   * Toggle given pin in shadow register. Call update() to write to shift
   * register.
   * @param[in] pin pin number.
   */
  void toggle(uint8_t pin)
    __attribute__((always_inline))
  {
    if (is_set(pin))
      clear(pin);
    else
      set(pin);
  }

  /**
   * Set the shadow registers. Call update() to write to shift
   * register.
   */
  void set()
    __attribute__((always_inline))
  {
    memset(m_port, 0xff, N);
  }

  /**
   * Clear the shadow registers. Call update() to write to shift
   * register.
   */
  void clear()
    __attribute__((always_inline))
  {
    memset(m_port, 0, N);
  }

  /**
   * Update shift register with value of shadow registers.
   */
  void update()
  {
    uint8_t ix = N;
    while (ix--) m_sda.write(m_port[ix], m_scl);
  }

  /**
   * Output pin in shift-register parallel output port.
   */
  class OutputPin {
  public:
    OutputPin(SRPO<N>* srpo, uint8_t pin) :
      m_srpo(srpo),
      m_pin(pin)
    {
    }

    /**
     * Set pin in shadow register. Call update() to write to shift
     * register.
     */
    void set()
      __attribute__((always_inline))
    {
      m_srpo->set(m_pin);
    }

  /**
   * Clear pin in shadow register. Call update() to write to shift
   * register.
   */
  void clear()
    __attribute__((always_inline))
  {
    m_srpo->clear(m_pin);
  }

  /**
   * Toggle pin in shadow register. Call update() to write to shift
   * register.
   */
  void toggle()
    __attribute__((always_inline))
  {
    m_srpo->toggle(m_pin);
  }

  protected:
    SRPO<N>* m_srpo;
    const uint8_t m_pin;
  };

protected:
  /** Shadow port register. */
  uint8_t m_port[N];

  /** Serial data output pin */
  ::OutputPin m_sda;

  /** Serial clock output pin */
  ::OutputPin m_scl;
};
};
#endif
