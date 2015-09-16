/**
 * @file IR.hh
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

#ifndef COSA_IR_HH
#define COSA_IR_HH

#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa IR receiver/TSOP4838 driver.
 *
 * @section Circuit
 * @code
 *                          TSOP4838
 *                       +------------+
 * (EXTn)--------------1-|OUT         |
 * (GND)---------------2-|GND    ( )  |
 * (VCC)---------------3-|VCC         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. http://www.vishay.com/docs/82459/tsop48.pdf
 */
class IR {
public:

  class Receiver : private ExternalInterrupt, public Job {
  public:
    /**
     * Mapping structure from code to key. Data structure
     * for storage in program memory.
     */
    struct keymap_t {
      uint16_t code;
      char key;
    };
    typedef const keymap_t* keymap_P;

    /**
     * @override{Interrupt::Handler}
     * Interrupt pin handler: Measure time periods of pulses in sequence
     * from IR receiver circuit. Push an event when a full sequence has
     * been recieved; READ_COMPLETED(this, code) where the code is the
     * recieved binary code or key if a key map was provided.
     * @param[in] arg argument from interrupt service routine.
     */
    virtual void on_interrupt(uint16_t arg = 0);

  public:
    /**
     * Construct an IR::Receiver connected to the given interrupt pin,
     * capture given max number of samples, and decode to binary
     * with the given threshold. A key map may be provided; pointer
     * to vector in program memory and size of vector. To collect
     * the time period samples provide a storage vector.
     * @param[in] pin interrupt pin (Board::EXTn).
     * @param[in] max number of samples.
     * @param[in] threshold level for mapping to binary.
     * @param[in] scheduler for timeout.
     * @param[in] keymap mapping table from code to key.
     * @param[in] keys number of members in keymap.
     * @param[in] sample vector for samples[max].
     */
    Receiver(Board::ExternalInterruptPin pin,
	     uint8_t max, uint32_t threshold,
	     Job::Scheduler* scheduler,
	     keymap_P keymap = NULL, uint8_t keys = 0,
	     uint16_t* sample = NULL) :
      ExternalInterrupt(pin, ExternalInterrupt::ON_FALLING_MODE),
      Job(scheduler),
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
     * Reset the receiver for the next code sequence.
     */
    void reset();

    /**
     * Lookup given code and return key or EOF(-1).
     * @param[in] code to lookup in key map.
     * @return key or EOF(-1).
     */
    int lookup(uint16_t code);

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
     * Print the captured samples to the given output stream.
     * @param[in] outs stream.
     * @param[in] receiver samples to print.
     * @return iostream.
     */
    friend IOStream& operator<<(IOStream& outs, Receiver& receiver);
  };
};

#endif

