/**
 * @file TCS230.hh
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

#ifndef COSA_TCS320_HH
#define COSA_TCS320_HH

#include "Cosa/OutputPin.hh"
#include "Cosa/ExternalInterrupt.hh"

/**
 * Cosa Device Driver for TCS230 Programmable Color Light-to-Frequency
 * Converter.
 *
 * @section Circuit
 * @code
 *                       TCS230 Module
 *                   P1 +------------+ P2
 * (D4)---------------1-|S0        S3|-1-----------------(D7)
 * (D5)---------------2-|S1        S2|-2-----------------(D6)
 *                    3-|OE       OUT|-3------------(D3/EXT1)
 * (GND)--------------4-|GND      VCC|-4----------------(VCC)
 *                      +------------+
 * @endcode
 *
 * @section References
 * 1. TCS230 Programmable Color Light-to-Frequency Converter, TAOS046,
 * Feb. 2003, http://www.pobot.org/IMG/pdf/tcs230_datasheet.pdf
 */
class TCS230 {
public:
  /**
   * Photodiode type setting, S2 and S3. (Table 1. Selectable Options,
   * pp. 2).
   */
  enum Filter {
    RED_FILTER = 0,
    BLUE_FILTER = 1,
    NO_FILTER = 2,
    GREEN_FILTER = 3
  };

  /**
   * Construct TCS230 device driver with given device output pin and
   * selectable option, output frequency scaling and photodiode type,
   * pins. The default setting is 20% scaling and GREEN.
   * @param[in] out device data pulse output (default EXT1).
   * @param[in] s0 frequency scaling option bit0 (default D4).
   * @param[in] s1 frequency scaling option bit1 (default D5).
   * @param[in] s2 photodiode type option bit0 (default D6).
   * @param[in] s3 photodiode type option bit0 (default D7).
   */
#if !defined(BOARD_ATTINY)
  TCS230(Board::ExternalInterruptPin out = Board::EXT1,
	 Board::DigitalPin s0 = Board::D4,
	 Board::DigitalPin s1 = Board::D5,
	 Board::DigitalPin s2 = Board::D6,
	 Board::DigitalPin s3 = Board::D7);
#else
  TCS230(Board::ExternalInterruptPin out = Board::EXT0,
	 Board::DigitalPin s0 = Board::D0,
	 Board::DigitalPin s1 = Board::D1,
	 Board::DigitalPin s2 = Board::D2,
	 Board::DigitalPin s3 = Board::D3);
#endif

  /**
   * Set output frequency scaling (f0). Power-down (0%), 2%, 20% and
   * 100% levels.
   * @param[in] percent output frequency scaling.
   */
  void frequency_scaling(uint8_t percent);

  /**
   * Set photodiode filter type (RED, GREEN, BLUE or NO_FILTER).
   * @param[in] type of color filter.
   */
  void photodiode(Filter type);

  /**
   * Collect generated pulses for the given time-period with the
   * current output frequency scaling and photodiode type. Return
   * number of pulses or UINT16_MAX if over-flow.
   * @return pulses.
   */
  uint16_t sample(uint8_t ms = 10);

private:
  class IRQPin : public ExternalInterrupt {
  public:
    static const uint16_t MAX = 10000;
    IRQPin(Board::ExternalInterruptPin pin);
    virtual void on_interrupt(uint16_t arg = 0);
    friend class TCS230;
  private:
    uint16_t m_count;
  };
  IRQPin m_out;
  OutputPin m_s0;
  OutputPin m_s1;
  OutputPin m_s2;
  OutputPin m_s3;
};

#endif


