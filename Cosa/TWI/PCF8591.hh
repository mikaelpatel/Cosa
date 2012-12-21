/**
 * @file Cosa/TWI/PCF8591.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Driver for the PCF8591 2-Wire 8-bit A/D and D/A converter.
 * See Philips Semiconductor product description (Rev. 2003 Jan 27).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TWI_PCF8591_HH__
#define __COSA_TWI_PCF8591_HH__

#include "Cosa/TWI.hh"

class PCF8591 : private TWI::Driver {

private:
  // Two-wire address for PCF8591 
  static const uint8_t ADDR = 0x90;
  uint8_t _addr;
  uint8_t _cntl;

public:
  /**
   * Control byte; selection of input channel and mode of operation
   */
  enum {
    AIN0 = 0x00,
    AIN1 = 0x01,
    AIN2 = 0x02,
    AIN3 = 0x03,
    CHANNEL_MASK = 0x03,
    AUTO_INCREMENT = 0x04,
    FOUR_INPUTS = 0x00,
    THREE_DIFF_INPUTS = 0x10,
    TWO_MIXED_INPUTS = 0x20,
    TWO_DIFF_INPUTS = 0x30,
    OUTPUT_ENABLE = 0x40
  };

  /**
   * Construct PCF8591 TWI A/D, D/A converter device access to given
   * chip address.
   * @param[in] addr chip address (0..7)
   */
  PCF8591(uint8_t addr = 0) : _addr(ADDR | (addr & 0xe)) {}

  /**
   * Begin a sampling sequence for the channel given by the control
   * parameter. Return true(1) if successful otherwise false(0).
   * @param[in] cntl control byte
   * @return bool
   */
  bool begin(uint8_t cntl);
  
  /**
   * End sampling sequence.
   */
  void end()
  {
    twi.end();
  }
  
  /**
   * Sample the channel defined by the latest begin() call.
   * Return sample value.
   * @return sample
   */
  uint8_t sample()
  {
    uint8_t res;
    twi.read(_addr, &res, 1);
    return (res);
  }

  /**
   * Read a sequence of samples the channel defined by the latest
   * begin() call. 
   * @param[in] buf sample buffer.
   * @param[in] size of sample buffer.
   * @return count or negative error code.
   */
  int sample(uint8_t* buf, uint8_t size)
  {
    return (twi.read(_addr, buf, size));
  }

  /**
   * Convert given value to analog output (voltage).
   * Return true(1) if successful otherwise false(0).
   * @param[in] value output value.
   * @return bool
   */
  bool convert(uint8_t value);
};

#endif
