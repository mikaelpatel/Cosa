/**
 * @file Cosa/Driver/NEXA.hh
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

#ifndef __COSA_DRIVER_NEXA_HH__
#define __COSA_DRIVER_NEXA_HH__

#include "Cosa/Types.h"
#include "Cosa/ExternalInterruptPin.hh"
#include "Cosa/IOStream.hh"

/**
 * NEXA Wireless Lighting Control receiver and transmitter.
 */
class NEXA {
public:
  /**
   * Wireless command code; 32-bit
   */
  union code_t {
    uint32_t as_long;
    struct {
      uint8_t unit:4;
      uint8_t onoff:1;
      uint8_t group:1;
      uint32_t house:26;
    };

    /**
     * Convert 32-bit number to command code.
     * @param[in] value to convert.
     */
    code_t(uint32_t value) 
    { 
      as_long = value; 
    }

    /**
     * Print command code fields to given output stream.
     * @param[in] outs output stream.
     */
    void println(IOStream& outs) 
    { 
      outs << PSTR("house = ") << house 
	   << PSTR(", group = ") << group
	   << PSTR(", unit = ") << unit
	   << PSTR(", on/off = ") << onoff 
	   << endl;
    }
  };
  
  /**
   * NEXA Wireless Remote Receiver. May be used in polling or
   * interrupt sampling mode.  
   */
  class Receiver : public ExternalInterruptPin {
  private:
    static const uint16_t SAMPLE_MAX = 129;
    static const uint16_t LOW_THRESHOLD = 200;
    static const uint16_t BIT_THRESHOLD = 500;
    static const uint16_t HIGH_THRESHOLD = 1500;
    volatile uint16_t m_sample[4];
    volatile uint32_t m_start;
    volatile uint32_t m_code;
    volatile uint8_t m_ix;

    /**
     * @override
     * Measures the pulse with and decodes the pulse stream. Will push
     * an Event::READ_COMPLETED_TYPE when completed decoding. Command
     * should be retrieved with get_code(). The event will contain the
     * object as target.
     * @param[in] arg argument from first level interrupt handler.
     */
    virtual void on_interrupt(uint16_t arg = 0);

    /**
     * Decode the current four samples. Return bit, zero(0) or one(1),
     * if successful otherwise negative error code.
     * @return decoded bit(0/1) or negative error code(-1).
     */
    int8_t decode_bit();

  public:
    /**
     * Create a NEXA::Receiver connected to the given external
     * interrupt pin. The interrupt handler must be enabled to become
     * active. Use enable() in setup().
     * @param[in] pin external interrupt pin.
     */
    Receiver(Board::ExternalInterruptPin pin) :
      ExternalInterruptPin(pin, ExternalInterruptPin::ON_CHANGE_MODE),
      m_start(0),
      m_code(0),
      m_ix(0)
    {}

    /**
     * Retrieve decoded command after Event::READ_COMPLETED_TYPE, from
     * interrupt handler.
     * @return decoded command.
     */
    code_t get_code() 
    { 
      return (m_code); 
    }

    /**
     * Poll wireless receiver for incoming command. Will busy-wait on
     * signal change.
     * @return received decoded command.
     */
    code_t read_code();
  };
};

#endif
