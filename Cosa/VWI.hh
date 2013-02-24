/**
 * @file Cosa/VWI.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2008-2013, Mike McCauley (Author/VirtualWire)
 * Copyright (C) 2013, Mikael Patel (Cosa C++ port and refactoring)
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
 * VWI (Virtual Wire Interface) is an Cosa library that provides 
 * features to send short messages, without addressing, retransmit or 
 * acknowledgment, a bit like UDP over wireless, using ASK (Amplitude 
 * Shift Keying). Supports a number of inexpensive radio transmitters
 * and receivers. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_VWI_HH__
#define __COSA_VWI_HH__

#include "Cosa/Pins.hh"
#include <avr/sleep.h>

extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal));

class VWI {
public:
  /** The maximum payload length */
  static const uint8_t PAYLOAD_MAX = 32;

  /** Maximum number of bytes in a message (incl. byte count and FCS) */
  static const uint8_t MESSAGE_MAX = PAYLOAD_MAX + 3;

  /** 
   * 4 bit to 6 bit symbol converter table. Used to convert the high
   * and low nybbles of the transmitted data into 6 bit symbols for
   * transmission. Each 6-bit symbol has 3 1s and 3 0s with at most 3
   * consecutive identical bits. 
   */
  static const uint8_t symbols[] PROGMEM;

  /**
   * Compute CRC over count bytes.
   * @param[in] ptr buffer pointer.
   * @param[in] count number of bytes in buffer.
   * @return CRC.
   */
  static uint16_t CRC(uint8_t* ptr, uint8_t count);

  /**
   * Convert a 6 bit encoded symbol into its 4 bit decoded
   * equivalent. 
   * @param[in] symbol 6-bit symbol.
   * @return 4-bit decoding.
   */
  static uint8_t symbol_6to4(uint8_t symbol);

  /** Sleep mode while synchronious await */
  static uint8_t s_mode;
  
public:
  /**
   * Initialise the Virtual Wire Interface (VWI), to operate at speed
   * bits per second with given sleep mode. Return true(1) if
   * successful otherwise false(0). Must be called before transmitting
   * or receiving.
   * @param[in] speed in bits per second.
   * @param[in] mode sleep mode.
   * @return bool
   */
  static bool begin(uint16_t speed, uint8_t mode = SLEEP_MODE_IDLE);

  class Receiver : private InputPin {
  private:
    /** The size of the receiver ramp. Ramp wraps modulo this number */
    static const uint8_t RAMP_MAX = 160;

    /* Number of samples per bit */
    static const uint8_t SAMPLES_PER_BIT = 8;

    /** 
     * Ramp adjustment parameters. Standard is if a transition occurs
     * before RAMP_TRANSITION(80) in the ramp, the ramp is retarded
     * by adding RAMP_INC_RETARD(11) else by adding
     * RAMP_INC_ADVANCE(29). If there is no transition it is adjusted
     * by RAMP_INC(20), Internal ramp adjustment parameter 
     */
    static const uint8_t RAMP_INC = RAMP_MAX / SAMPLES_PER_BIT;

    /** Internal ramp adjustment parameter */
    static const uint8_t RAMP_TRANSITION = RAMP_MAX / 2;

    /** Internal ramp adjustment parameter */
    static const uint8_t RAMP_ADJUST = 9;
    
    /** Internal ramp adjustment parameter */
    static const uint8_t RAMP_INC_RETARD = (RAMP_INC - RAMP_ADJUST);
    
    /** Internal ramp adjustment parameter */
    static const uint8_t RAMP_INC_ADVANCE = (RAMP_INC + RAMP_ADJUST);

    /** Current receiver sample */
    uint8_t m_sample;

    /** Last receiver sample */
    uint8_t m_last_sample;

    /** 
     * PLL ramp, varies between 0 and RAMP_LEN-1(159) over 
     * SAMPLES_PER_BIT (8) samples per nominal bit time. When the PLL
     * is synchronised, bit transitions happen at about the 0 mark. 
     */
    uint8_t m_pll_ramp;

    /**
     * This is the integrate and dump integral. If there are <5 0
     * samples in the PLL cycle the bit is declared a 0, else a 1
     */
    uint8_t m_integrator;

    /**
     * Flag indictate if we have seen the start symbol of a new
     * message and are in the processes of reading and decoding it 
     */
    uint8_t m_active;

    /** Flag to indicate that a new message is available */
    volatile uint8_t m_done;

    /** Flag to indicate the receiver PLL is to run */
    uint8_t m_enabled;

    /** Last 12 bits received, so we can look for the start symbol */
    uint16_t m_bits;

    /** How many bits of message we have received. Ranges from 0 to 12 */
    uint8_t m_bit_count;

    /** The incoming message buffer */
    uint8_t m_buffer[VWI::MESSAGE_MAX];

    /** The incoming message expected length */
    uint8_t m_count;

    /** The incoming message buffer length received so far */
    volatile uint8_t m_length;

    /** Number of bad messages received and dropped due to bad lengths */
    uint8_t m_bad;

    /** Number of good messages received */
    uint8_t m_good;

    /** The interrupt handler is a friend */
    friend void TIMER1_COMPA_vect(void);

    /**
     * Phase locked loop tries to synchronise with the transmitter so
     * that bit transitions occur at about the time (m_pll_ramp) is
     * 0; Then the average is computed over each bit period to deduce
     * the bit value 
     */
    void PLL();

  public:
    Receiver(Board::DigitalPin rx);

    /**
     * Start the Phase Locked Loop listening for the receiver.
     * Must do this before you can receive any messages, When a
     * message is available (good checksum or not), available(), 
     * will return non-zero.
     * @return bool
     */
    bool begin()
    {
      if (!m_enabled) {
	m_enabled = true;
	m_active = false;
      }
      return (1);
    }

    /**
     * Stop the Phase Locked Loop listening to the receiver. No
     * messages will be received until begin() is called
     * again. Saves interrupt processing cycles.
     * @return bool
     */
    bool end()
    {
      m_enabled = false;
      return (1);
    }

    /**
     * Block until a message is available or for a max time (0 to
     * forever). 
     * @param[in] ms maximum time to wait in milliseconds.
     * @return bool, true if a message is available, false if the wait
     * timed out.
     */
    bool await(uint32_t ms = 0);

    /**
     * Returns true if an unread message is available. May have a
     * back check-sum.
     * @return true(1) if a message is available to read.
     */
    uint8_t available()
    {
      return (m_done);
    }

    /**
     * If a message is available (good checksum or not), copies up to
     * len bytes to the given buffer, buf. 
     * @param[in] buf pointer to location to save the read data.
     * @param[in] len available space in buf. 
     * @param[in] ms timeout period (zero for non-blocking)
     * @return number of bytes received or negative error code.
     */
    int8_t recv(void* buf, uint8_t len, uint32_t ms = 0);
  };

  class Transmitter : private OutputPin {
  private:
    /** 
     * Outgoing message bits grouped as 6-bit words, 36 alternating 1/0
     * bits, followed by 12 bits of start symbol. Followed immediately
     * by the 4-6 bit encoded byte count, message buffer and 2 byte
     * FCS. Each byte from the byte count on is translated into 2x6-bit
     * words. Caution, each symbol is transmitted LSBit first, but each
     * byte is transmitted high nybble first.
     */
    static const uint8_t header[] PROGMEM;

    /** Size of header */
    static const uint8_t HEADER_MAX = 8;

    /** Transmission buffer with symbols and header */
    uint8_t m_buffer[(MESSAGE_MAX * 2) + HEADER_MAX];

    /** Number of symbols to be sent */
    uint8_t m_length;

    /** Index of the next symbol to send. Ranges from 0..length-1 */
    uint8_t m_index;

    /** Bit number of next bit to send */
    uint8_t m_bit;

    /** Sample number for the transmitter, 0..7 in one bit interval */
    uint8_t m_sample;

    /** Flag to indicated the transmitter is active */
    volatile uint8_t m_enabled;

    /** Total number of messages sent */
    uint16_t m_msg_count;

    /** The interrupt handler is a friend */
    friend void TIMER1_COMPA_vect(void);

  public:
    Transmitter(Board::DigitalPin tx);

    /**
     * Start transmitter. Returns true(1) if successful otherwise false(0).
     */
    bool begin();
    
    /**
     * Stop transmitter. Returns true(1) if successful otherwise false(0).
     */
    bool end()
    {
      clear();
      m_enabled = false;
      return (1);
    }

    /**
     * Returns the state of the transmitter.
     * @return true if the transmitter is active else false
     */
    bool is_active()
    {
      return (m_enabled);
    }

    /**
     * Block until the transmitter is idle then returns.
     */
    void await();

    /**
     * Send a message with the given length. Returns almost
     * immediately, and message will be sent at the right timing by
     * interrupts. 
     * @param[in] buf pointer to the data to transmit.
     * @param[in] len number of octetes to transmit.
     * @return true if the message was accepted for transmission,
     * false if the message is too long (> PAYLOAD_MAX) 
     */
    bool send(void* buf, uint8_t len);
  };
};

#endif
