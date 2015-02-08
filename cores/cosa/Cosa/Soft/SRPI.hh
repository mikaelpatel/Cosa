/**
 * @file Cosa/Soft/SRPI.hh
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

#ifndef COSA_SOFT_SRPI_HH
#define COSA_SOFT_SRPI_HH

#include "Cosa/OutputPin.hh"
#include "Cosa/InputPin.hh"

namespace Soft {

/**
 * Soft N-Shift Register Parallel Input. The shift registers (74HC165)
 * may be cascaded for N*8-bit parallel input port (see circuit
 * below). The pins are numbered from the first connect shift register
 * (Q0..Q7) and updwards in the chain (Q8..Q15) and so on.
 *
 * @section Circuit
 * @code
 *                         74HC165    (VCC)
 *                       +----U----+    |
 * (D5/PLD)---+--------1-|/PL   VCC|-16-+
 * (D4/SCL)-+-)--------2-|CP    /CE|-15-----------(GND)
 * (Q4)-----)-)--------3-|D4     D3|-14------------(Q3)
 * (Q5)-----)-)--------4-|D5     D2|-13------------(Q2)
 * (Q6)-----)-)--------5-|D6     D1|-12------------(Q1)
 * (Q7)-----)-)--------6-|D7     D0|-11------------(Q0)
 *          | |        7-|/Q7    DS|-10-----------------+
 *          | |      +-8-|GND    Q7|--9--------(SDA/D3) |
 *          | |      |   +---------+                    |
 *          | |      |      0.1uF                       |
 *          | |    (GND)-----||-------(VCC)             |
 *          | |                         |               |
 *          | |            74HC165      |               |
 *          | |          +----U----+    |               |
 *          | +--------1-|/PL   VCC|-16-+               |
 *          +-)--------2-|CP    /CE|-15-----------(GND) |
 * (Q12)----)-)--------3-|D4     D3|-14-----------(Q11) |
 * (Q13)----)-)--------4-|D5     D2|-13-----------(Q10) |
 * (Q14)----)-)--------5-|D6     D1|-12------------(Q9) |
 * (Q15)----)-)--------6-|D7     D0|-11------------(Q8) |
 *          | |        7-|/Q7    DS|-10-----------------)--+
 *          | |      +-8-|GND    Q7|--9-----------------+  |
 *          | |      |   +---------+                       |
 *          | |      |      0.1uF                          |
 *          v v    (GND)-----||-------(VCC)                v
 * @endcode
 *
 * @param[in] N number of shift registers (N * 8 input pins).
 */
template<uint8_t N>
class SRPI {
public:
  /** Number of pins for N ports */
  static const uint8_t PINS = N * CHARBITS;

  /**
   * Construct N-shift register connected to given pins.
   * @param[in] sda serial input data (Default D3).
   * @param[in] scl serial clock (Default D4).
   * @param[in] pld load (Default D5).
   */
  SRPI(Board::DigitalPin sda = Board::D3,
       Board::DigitalPin scl = Board::D4,
       Board::DigitalPin pld = Board::D5) :
    m_sda(sda),
    m_scl(scl),
    m_pld(pld, 1)
  {
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
   * Update shadow register with value of shift registers.
   * Parallel load and shift from register.
   */
  void update()
  {
    m_pld.toggle();
    m_pld.toggle();
    for (uint8_t ix = 0; ix < N; ix++)
      m_port[ix] = m_sda.read(m_scl);
  }

  /**
   * Output pin in shift-register parallel output port.
   */
  class InputPin {
  public:
    InputPin(SRPI<N>* srpi, uint8_t pin) :
      m_srpi(srpi),
      m_pin(pin)
    {
    }

    /**
     * Return true(1) if the given pin in shadow register is set,
     * otherwise false(0).
     * @return bool.
     */
    bool is_set()
      __attribute__((always_inline))
    {
      return (m_srpi->is_set(m_pin));
    }

    /**
     * Return true(1) if the given pin in shadow register is set,
     * otherwise false(0).
     * @return bool.
     */
    void is_clear(uint8_t pin)
      __attribute__((always_inline))
    {
      return (m_srpi->is_clear(m_pin));
    }

  protected:
    SRPI<N>* m_srpi;
    const uint8_t m_pin;
  };

protected:
  /** Shadow port register; LSB..MSB byte */
  uint8_t m_port[N];

  /** Serial data input pin */
  ::InputPin m_sda;

  /** Serial clock output pin */
  OutputPin m_scl;

  /** Parallel load output pin */
  OutputPin m_pld;
};
};
#endif
