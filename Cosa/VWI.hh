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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_VWI_HH__
#define __COSA_VWI_HH__

#include "Cosa/Pins.hh"
#include <avr/sleep.h>

/**
 * VWI (Virtual Wire Interface) is an Cosa library that provides
 * features to send short messages using inexpensive radio
 * transmitters and receivers (RF433).
 */
class VWI {
public:
  /** 
   * The maximum payload length; 32 byte application payload and 
   * 4 byte extended mode header (sizeof(header_t)) 
   */
  static const uint8_t PAYLOAD_MAX = 36;
  
  /** Maximum number of bytes in a message (incl. byte count and FCS) */
  static const uint8_t MESSAGE_MAX = PAYLOAD_MAX + 3;

  /** Minimum number of bytes in a message */
  static const uint8_t MESSAGE_MIN = 4;

  /** Number of samples per bit */
  static const uint8_t SAMPLES_PER_BIT = 8;

  /** CRC checksum for received frame */
  static const uint16_t CHECK_SUM = 0xf0b8;

  /**
   * Compute CRC over count bytes and return value. Used for checking
   * received messages. Return value should be CHECK_SUM.
   * @param[in] ptr buffer pointer.
   * @param[in] count number of bytes in buffer.
   * @return CRC.
   */
  static uint16_t CRC(uint8_t* ptr, uint8_t count);

  /** Message header for extended Virtual Wire Interface mode */
  struct header_t {
    uint16_t addr;		/**< Transmitter node address */
    uint8_t cmd;		/**< Command or message type */
    uint8_t nr;			/**< Message sequence number */
  };

  /**
   * Initialise the Virtual Wire Interface (VWI), to operate at given
   * speed bits per second and with given sleep mode. Return true(1)
   * if successful otherwise false(0). Must be called before
   * transmitting or receiving. Default sleep mode is idle.
   * @param[in] speed in bits per second.
   * @param[in] mode sleep mode (Default SLEEP_MODE_IDLE).
   * @return bool
   */
  static bool begin(uint16_t speed, uint8_t mode = SLEEP_MODE_IDLE);

  /**
   * Initialise the Virtual Wire Interface (VWI) for extended mode;
   * include message header (VWI::header_t). Initiate with the given
   * node address to operate at speed bits per second and with given
   * sleep mode. Return true(1) if successful otherwise false(0). Must
   * be called before transmitting or receiving. Default sleep mode is
   * idle. 
   * @param[in] addr node address.
   * @param[in] speed in bits per second.
   * @param[in] mode sleep mode (Default SLEEP_MODE_IDLE) 
   * @return bool
   */
  static bool begin(uint16_t addr, uint16_t speed, uint8_t mode = SLEEP_MODE_IDLE)
  {
    s_addr = addr;
    return (begin(speed, mode));
  }

  /**
   * Enable the Virtual Wire Interface (VWI) interrupt handling after
   * deep sleep modes.
   */
  static void enable();

  /**
   * Disable the Virtual Wire Interface (VWI) interrupt handling for
   * deep sleep modes.
   */
  static void disable();

private:
  /** Sleep mode during await */
  static uint8_t s_mode;
  
  /** Node address used in extended mode in message header */
  static uint32_t s_addr;

public:
  /**
   * The Virtual Wire Codec; define message preamble and start
   * symbol. Supports encode and decoding of data to transmission
   * symbols. Cosa support several transmission codecs. They may be
   * used to optimize performance in a given scenario; speed, noise,
   * message length, etc.
   */
  class Codec {
  public:
    /** Bits per symbol */
    const uint8_t BITS_PER_SYMBOL;
  
    /** Start symbol */
    const uint16_t START_SYMBOL;

    /** 
     * Size of preamble with start symbol. Should be less or equal to 
     * Transmitter::PREAMBLE_MAX.
     */
    const uint8_t PREAMBLE_MAX;

    /** Symbol mask */
    const uint8_t SYMBOL_MASK;

    /** Symbol MSB */
    const uint16_t BITS_MSB;

    /**
     * Construct Codec with given symbol and preamble definition. The 
     * Codec is assumed to code 4 bits to max 8 bit symbol for
     * transmission.
     * @param[in] bits_per_symbol.
     * @param[in] start_symbol.
     * @param[in] preamble_max.
     */
    Codec(uint8_t bits_per_symbol, uint16_t start_symbol, 
	  uint8_t preamble_max = VWI::Transmitter::PREAMBLE_MAX) :
      BITS_PER_SYMBOL(bits_per_symbol),
      START_SYMBOL(start_symbol),
      PREAMBLE_MAX(preamble_max),
      SYMBOL_MASK((1 << bits_per_symbol) - 1),
      BITS_MSB(1 << (bits_per_symbol*2 - 1))
    {}

    /**
     * Provide pointer to frame preamble in program memory. PREAMBLE_MAX 
     * should contain the length of the preamble including start symbol. 
     * @return pointer to program memory.
     */
    virtual const uint8_t* get_preamble() = 0;

    /**
     * Encode 4 bits (nibble) to a symbol with BITS_PER_SYMBOL.
     * @param[in] nibble data to encode.
     * @return symbol.
     */
    virtual uint8_t encode4(uint8_t nibble) = 0;

    /**
     * Decode symbol back to 4 bits (nibble) of data.
     * @param[in] symbol to decode.
     * @return data.
     */
    virtual uint8_t decode4(uint8_t symbol) = 0;

    /**
     * Decode two packed symbols (max 16-bit) back to 8 bits (byte) of
     * data.
     * @param[in] symbol to decode.
     * @return data.
     */
    virtual uint8_t decode8(uint16_t symbol)
    {
      return ((decode4(symbol) << 4) | (decode4(symbol >> BITS_PER_SYMBOL)));
    }
  };

  /**
   * The Virtual Wire Receiver.
   */
  class Receiver : private InputPin {
  private:
    /** The size of the receiver ramp. Ramp wraps modulo this number */
    static const uint8_t RAMP_MAX = 160;

    /** Number of samples to integrate before mapping to one(1) */
    static const uint8_t INTEGRATOR_THRESHOLD = 5;

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
    Codec* m_codec;

    /** Sub-net mask for extended mode address match */
    uint16_t m_mask;

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

    /** How many bits of message we have received? Ranges from 0 to 12 */
    uint8_t m_bit_count;

    /** The incoming message buffer */
    uint8_t m_buffer[MESSAGE_MAX];

    /** The incoming message expected length */
    uint8_t m_count;

    /** The incoming message buffer length received so far */
    volatile uint8_t m_length;

    /** Number of bad messages received and dropped due to bad lengths */
    uint16_t m_bad;

    /** Number of good messages received */
    uint16_t m_good;

    /** The interrupt handler is a friend */
    friend void TIMER1_COMPA_vect(void);

    /**
     * Phase Locked Loop; Synchronises with the transmitter so that
     * bit transitions occur at about the time (m_pll_ramp) is 0, then
     * the average is computed over each bit period to deduce the bit value.
     */
    void PLL();

  public:
    /**
     * Construct VWI Receiver instance connected to the given pin.
     * @param[in] rx input pin.
     * @param[in] codec for the receiver.
     */
    Receiver(Board::DigitalPin pin, Codec* codec);

    /**
     * Start the Phase Locked Loop listening for the receiver. Must do
     * this before receiving any messages, When a message is available
     * (good checksum or not), available(), will return true.
     * @param[in] mask for sub-net address match.
     * @return bool
     */
    bool begin(uint16_t mask = 0xffffU);
    
    /**
     * Stop the Phase Locked Loop listening to the receiver. No
     * messages will be received until begin() is called again. Saves
     * interrupt processing cycles. 
     * @return bool
     */
    bool end();

    /**
     * Block until a message is available or for a max time (default
     * 0L for block forever). Returns true if a message is available,
     * false if the wait timed out.
     * @param[in] ms maximum time to wait in milliseconds.
     * @return bool.
     */
    bool await(uint32_t ms = 0L);

    /**
     * Returns true if an unread message is available. May have a
     * bad check-sum.
     * @return true(1) if a message is available to read.
     */
    bool available()
    {
      return (m_done);
    }

    /**
     * Set sub-net mask in extended mode.
     * @param[in] mask for sub-net address match.
     */
    uint16_t set_subnet_mask(uint16_t mask = 0xffffU)
    {
      uint32_t previous = m_mask;
      m_mask = mask;
      return (previous);
    }

    /**
     * Get receiver statistics; number of good and bad messages.
     */
    void get_stat(uint16_t& good, uint16_t& bad)
    {
      good = m_good;
      bad = m_bad;
    }
    
    /**
     * If a message is available (good checksum or not), copies up to
     * len bytes to the given buffer, buf. 
     * @param[in] buf pointer to location to save the read data.
     * @param[in] len available space in buf. 
     * @param[in] ms timeout period (zero for non-blocking)
     * @return number of bytes received or negative error code.
     */
    int8_t recv(void* buf, uint8_t len, uint32_t ms = 0L);
  };

  /**
   * The Virtual Wire Transmitter.
   */
  class Transmitter : private OutputPin {
  private:
    /** Max size of preamble and start symbol. Codec provides actual size */
    static const uint8_t PREAMBLE_MAX = 8;
    
    /** Transmission buffer with premable, start symbol, count and payload */
    uint8_t m_buffer[(MESSAGE_MAX * 2) + PREAMBLE_MAX];

    /** Current transmitter codec */
    Codec* m_codec;

    /** Message sequence number for extended mode */
    uint8_t m_nr;

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
    uint16_t m_count;

    /** The interrupt handler is a friend */
    friend void TIMER1_COMPA_vect(void);

    /** Allow Codec access to PREAMBLE_MAX */
    friend class Codec;

  public:
    /**
     * Construct VWI Transmitter instance connected to the given
     * pin. Use given codec for encoding data.
     * @param[in] pin transmitter input pin.
     * @param[in] codec for transmitter.
     */
    Transmitter(Board::DigitalPin pin, Codec* codec);

    /**
     * Start transmitter. Returns true(1) if successful otherwise false(0).
     * @return bool
     */
    bool begin();
    
    /**
     * Stop transmitter. Returns true(1) if successful otherwise false(0).
     */
    bool end()
    {
      clear();
      m_enabled = false;
      return (true);
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
     * Get next message sequence number in extended mode.
     * @return message sequence number.
     */
    uint8_t get_next_nr()
    {
      return (m_nr);
    }

    /**
     * Set next message sequence number in extended mode.
     * @param[in] next sequence number.
     */
    void set_next_nr(uint8_t value)
    {
      m_nr = value;
    }

    /**
     * Get transmitter statistics; number of send messages
     */
    void get_stat(uint16_t& count)
    {
      count = m_count;
    }
    
    /**
     * Block until the transmitter is idle then returns.
     */
    void await();

    /**
     * Send message using a null terminated io vector message. Returns
     * almost immediately, and message will be sent at the right
     * timing by interrupts. Message is gathered from elements in io
     * vector. The total size of the io vector buffers must be less
     * than PAYLOAD_MAX.
     * @param[in] vec null terminated io vector.
     * @return true(1) if accepted for transmission, otherwise false(0).
     */
    bool send(const iovec_t* vec);

    /**
     * Send a message with the given length. Returns almost
     * immediately, and message will be sent at the right timing by
     * interrupts. A command may be given in extended mode with
     * addressing to allow identification of the message type.
     * The message length (len) must be less than PAYLOAD_MAX.
     * @param[in] buf pointer to the data to transmit.
     * @param[in] len number of bytes to transmit.
     * @param[in] cmd command code in extended mode.
     * @return true(1) if accepted for transmission, otherwise false(0).
     */
    bool send(const void* buf, uint8_t len, uint8_t cmd = 0);

    /**
     * Resend previous message. Return true(1) if successful otherwise
     * false(0).
     * @return bool
     */
    bool resend();
  };

  /**
   * Virtual Wire Transceiver with message acknowledgement and
   * retransmission.
   */
  class Transceiver {
  public:
    /** Maximum size of extended mode payload (32 bytes) */
    static const uint8_t PAYLOAD_MAX = VWI::PAYLOAD_MAX - sizeof(header_t);

    /** Maximum number of retransmission */
    static const uint8_t RETRANS_MAX = 16;

    /** Timeout on acknowledge receive */
    static const uint32_t TIMEOUT = 500;

    /** Set command as not acknowledged */
    static const uint8_t NACK = 0x80;	

    /** Receiver member variable */
    Receiver rx;

    /** Transmitter member variable */
    Transmitter tx;

    /**
     * Transceiver constructor given transmitter and receiver pin, and
     * codec.
     * @param[in] rx_pin receiver pin.
     * @param[in] tx_pin transmitter pin.
     * @param[in] codec encoder/decoder.
     */
    Transceiver(Board::DigitalPin rx_pin, Board::DigitalPin tx_pin, Codec* codec);

    /**
     * Start the Phase Locked Loop listening for the receiver and
     * start transmitter.
     * @param[in] mask for sub-net address match.
     * @return bool
     */
    bool begin(uint16_t mask = 0xffffU);

    /**
     * Stop transmitter and receiver. Returns true(1) if successful
     * otherwise false(0). 
     */
    bool end();

    /**
     * If a message is available (good checksum or not), copies up to
     * len bytes to the given buffer, buf. Sends an acknowledgement.
     * Always received with extended mode header. Caller must check
     * that the message is not a retransmission by checking the
     * message sequence number. Returns number of bytes received.
     * @param[in] buf pointer to location to save the read data.
     * @param[in] len available space in buf. 
     * @param[in] ms timeout period (zero for non-blocking)
     * @return number of bytes received or negative error code.
     */
    int8_t recv(void* buf, uint8_t len, uint32_t ms = 0L);
    
    /**
     * Send a message with the given length, and await acknowledgement
     * if the nack flag is zero. The message will be retransmitted
     * until an acknowledgement is received. The message length (len)
     * must be less than PAYLOAD_MAX. The command code/message type
     * must be 0..127. Higher command code will mark the message as
     * non-acknowledged (nack != 0). Returns number of transmissions 
     * or negative error code(-1) if maximum number of retransmissions
     * RETRANS_MAX exceeded.
     * @param[in] buf pointer to the data to transmit.
     * @param[in] len number of bytes to transmit.
     * @param[in] cmd command code in extended mode (0..127).
     * @param[in] nack non zero for no acknowledgement (Default zero).
     * @return number of transmissions (1..n) otherwise negative error
     * code.
     */
    int8_t send(const void* buf, uint8_t len, uint8_t cmd, uint8_t nack = 0);
  };
};

#endif
