/**
 * @file Cosa/IR.hh
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
 * Cosa IR receiver/TSOP4838 driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IR_HH__
#define __COSA_IR_HH__

#include "Cosa/Pins.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Trace.hh"

class IR {
public:

  class Receiver : private InterruptPin, private Link {
  public:
    struct keymap_t {
      uint16_t code;
      char key;
    };
    typedef struct keymap_t keymap_t;
    typedef const PROGMEM keymap_t* keymap_P;

  private:
    static const uint16_t TIMEOUT = 512;
    volatile uint32_t m_threshold;
    volatile uint16_t* m_sample;
    volatile uint32_t m_start;
    volatile uint32_t m_code;
    volatile uint8_t m_ix;
    const uint8_t m_max;
    const keymap_P m_keymap;
    const uint8_t m_keys;

    /**
     * @override
     * Interrupt pin handler: Measure time periods of pulses in sequence 
     * from IR receiver circuit. Push an event when a full sequence has
     * been recieved. 
     */
    virtual void on_interrupt();

  public:
    /**
     * Construct an IRreceiver connected to the given interrupt pin,
     * capture given max number of samples, and decode to binary
     * with the given threshold. 
     * @param[in] pin interrupt pin (EXTn).
     * @param[in] max number of samples.
     * @param[in] threshold level for mapping to binary.
     * @param[in] keymap mapping table from code to key.
     * @param[in] keys number of members in keymap.
     * @param[in] sample vector for samples.
     */
    Receiver(Board::InterruptPin pin, uint8_t max, uint32_t threshold,
	     keymap_P keymap = 0, uint8_t keys = 0,
	     uint16_t* sample = 0) :
      InterruptPin(pin, InterruptPin::ON_FALLING_MODE),
      Link(),
      m_threshold(threshold),
      m_sample(sample),
      m_start(0),
      m_code(0),
      m_ix(0),
      m_max(max),
      m_keymap(keymap),
      m_keys(keys)
    {}

    /**
     * Reset the IR receive for the next code sequence.
     */
    void reset();

    /**
     * Print the captured samples to the given output stream.
     * @param[in] out stream (Default trace).
     */
    void print(IOStream& out = trace);

    /**
     * Lookup given code and return key or EOF(-1).
     * @param[in] code to lookup in key map.
     * @return key or EOF(-1).
     */
    char lookup(uint16_t code);
  };
};

#endif

