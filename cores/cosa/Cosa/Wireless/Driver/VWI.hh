/**
 * @file Cosa/Wireless/Driver/VWI.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2008-2013, Mike McCauley (Author/VirtualWire rev. 1.19)
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

#ifndef __COSA_WIRELESS_DRIVER_VWI_HH__
#define __COSA_WIRELESS_DRIVER_VWI_HH__

#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Wireless.hh"

/**
 * VWI is an Cosa library that provides features to send short
 * messages using inexpensive radio transmitters and receivers (VWI). 
 * This library is an object-oriented refactoring and extension of the
 * Virtual Wire library. 
 *
 * @section Circuit
 * @code
 *                         RF433/RX                       V
 *                       +------------+                   |
 *                       |0-----------|-------------------+
 *                       |ANT         |       17.3 cm
 *                       |            |
 *                       |            |
 *                       |            |
 *                       |            |
 * (VCC)---------------1-|VCC         |
 *                     2-|DATA        |
 * (RX)----------------3-|DATA        |
 * (GND)---------------4-|GND         |
 *                       +------------+
 *
 *                         RF433/TX
 *                       +------------+
 * (TX)----------------1-|DATA        |
 * (VCC)---------------2-|VCC         |                    V
 * (GND)---------------3-|GND         |                    |
 *                       |ANT       0-|--------------------+
 *                       +------------+       17.3 cm
 * @endcode
 */
class VWI : public Wireless::Driver {
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
     * @override VWI::Codec
     * Provide pointer to frame preamble in program memory. PREAMBLE_MAX 
     * should contain the length of the preamble including start symbol. 
     * @return pointer to program memory.
     */
    virtual const uint8_t* get_preamble() = 0;

    /**
     * @override VWI::Codec
     * Encode 4 bits (nibble) to a symbol with BITS_PER_SYMBOL.
     * @param[in] nibble data to encode.
     * @return symbol.
     */
    virtual uint8_t encode4(uint8_t nibble) = 0;

    /**
     * @override VWI::Codec
     * Decode symbol back to 4 bits (nibble) of data.
     * @param[in] symbol to decode.
     * @return data.
     */
    virtual uint8_t decode4(uint8_t symbol) = 0;

    /**
     * @override VWI::Codec
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

public:
  /**
   * Construct Virtual Wire Interface with given network, device
   * address and speed (bits per second). Attach Receiver to given rx
   * pin and Transmitter to tx pin. Use the given Codec for coding and
   * decoding messages.
   */
  VWI(int16_t net, uint8_t dev, 
      uint16_t speed, 
      Board::DigitalPin rx, 
      Board::DigitalPin tx,
      Codec* codec) :
    Wireless::Driver(net, dev),
    m_rx(rx, codec),
    m_tx(tx, codec),
    m_speed(speed)
  {
    s_rf = this;
  }

  /**
   * @override Wireless::Driver
   * Start the Wireless device driver. Return true(1) if successful
   * otherwise false(0).
   * @param[in] config configuration vector (default NULL)
   * @return bool
   */
  virtual bool begin(const void* config = NULL);

  /**
   * @override Wireless::Driver
   * Shut down the device driver. Return true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  virtual bool end();
    
  /**
   * @override Wireless::Driver
   * Set device in power up mode. 
   */
  virtual void powerup();

  /**
   * @override Wireless::Driver
   * Set device in power down mode. 
   */
  virtual void powerdown();

  /**
   * @override Wireless::Driver
   * Return true(1) if a message is available otherwise false(0).
   * @return bool
   */
  virtual bool available()
  {
    return (m_rx.available());
  }

  /**
   * @override Wireless::Driver
   * Send message in given null terminated io vector. Returns number
   * of bytes sent. Returns error code(-1) if number of bytes is
   * greater than PAYLOAD_MAX. Return error code(-2) if fails to set
   * transmit mode.
   * @param[in] dest destination network address.
   * @param[in] port device port (or message type).
   * @param[in] vec null termianted io vector.
   * @return number of bytes send or negative error code.
   */
  virtual int send(uint8_t dest, uint8_t port, const iovec_t* vec)
  {
    return (m_tx.send(dest, port, vec));
  }

  /**
   * @override Wireless::Driver
   * Send message in given buffer, with given number of bytes. Returns
   * number of bytes sent. Returns error code(-1) if number of bytes
   * is greater than PAYLOAD_MAX. Return error code(-2) if fails to
   * set transmit mode.  
   * @param[in] dest destination network address.
   * @param[in] port device port (or message type).
   * @param[in] buf buffer to transmit.
   * @param[in] len number of bytes in buffer.
   * @return number of bytes send or negative error code.
   */
  virtual int send(uint8_t dest, uint8_t port, const void* buf, size_t len)
  {
    return (m_tx.send(dest, port, buf, len));
  }

  /**
   * @override Wireless::Driver
   * Receive message and store into given buffer with given maximum
   * length. The source network address is returned in the parameter src.
   * Returns error code(-2) if no message is available and/or a
   * timeout occured. Returns error code(-1) if the buffer size if to
   * small for incoming message or if the receiver fifo has overflowed. 
   * Otherwise the actual number of received bytes is returned
   * @param[out] src source network address.
   * @param[out] port device port (or message type).
   * @param[in] buf buffer to store incoming message.
   * @param[in] len maximum number of bytes to receive.
   * @param[in] ms maximum time out period.
   * @return number of bytes received or negative error code.
   */
  virtual int recv(uint8_t& src, uint8_t& port, 
		   void* buf, size_t len, 
		   uint32_t ms = 0L)
  {
    return (m_rx.recv(src, port, buf, len, ms));
  }

private:
  /**
   * Frame header; Transmitted in little endian order; network LSB first.
   */
  struct header_t {
    int16_t network;		//<! Network address
    uint8_t dest;		//<! Destination device address
    uint8_t src;		//<! Source device address
    uint8_t port;		//<! Port or message type
  };
  
  /** 
   * The maximum payload length; 30 byte application payload and 
   * 4 byte frame header with network(2)
   */
  static const uint8_t PAYLOAD_MAX = 30 + sizeof(header_t);
  
  /** Maximum number of bytes in a message (incl. byte count and FCS) */
  static const uint8_t MESSAGE_MAX = PAYLOAD_MAX + 3;

  /** Minimum number of bytes in a message */
  static const uint8_t MESSAGE_MIN = sizeof(header_t);

  /** Number of samples per bit */
  static const uint8_t SAMPLES_PER_BIT = 8;

  /**
   * Internal Virtual Wire Receiver.
   */
  class Receiver : private InputPin {
  public:
    /**
     * Construct VWI Receiver instance connected to the given pin.
     * @param[in] rx input pin.
     * @param[in] codec for the receiver.
     */
    Receiver(Board::DigitalPin pin, Codec* codec) :
      InputPin(pin),
      m_codec(codec)
    {
    }
    
    /**
     * Start the Phase Locked Loop listening for the receiver. Must do
     * this before receiving any messages, 
     */
    void begin()
    {
      m_enabled = true;
      m_active = false;
    }
    
    /**
     * Stop the Phase Locked Loop listening to the receiver. No
     * messages will be received until begin() is called again. Saves
     * interrupt processing cycles. 
     */
    void end()
    {
      m_enabled = false;
    }

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
     * If a message is available (good checksum or not), copies up to
     * len bytes to the given buffer, buf. Returns number of bytes 
     * received/copied, zero(0) for timeout or negative error code; 
     * bad checksum(-1), and in enhanced mode did not match address(-2).
     * @param[out] src source network address.
     * @param[out] port device port (or message type).
     * @param[in] buf pointer to location to save the read data.
     * @param[in] len available space in buf. 
     * @param[in] ms timeout period (zero for blocking)
     * @return number of bytes received or negative error code.
     */
    int recv(uint8_t& src, uint8_t& port, 
	     void* buf, size_t len, 
	     uint32_t ms = 0L);

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

    /**
     * Phase Locked Loop; Synchronizes with the transmitter so that
     * bit transitions occur at about the time (m_pll_ramp) is 0, then
     * the average is computed over each bit period to deduce the bit value.
     */
    void PLL();

    /** Interrupt Service Routine */
    friend void TIMER1_COMPA_vect(void);
  };

  /**
   * Internal Virtual Wire Transmitter.
   */
  class Transmitter : private OutputPin {
  public:
    /**
     * Construct VWI Transmitter instance connected to the given
     * pin. Use given codec for encoding data.
     * @param[in] pin transmitter input pin.
     * @param[in] codec for transmitter.
     */
    Transmitter(Board::DigitalPin pin, Codec* codec) :
      OutputPin(pin),
      m_codec(codec)
    {
      memcpy_P(m_buffer, codec->get_preamble(), codec->PREAMBLE_MAX);
    }

    /**
     * Start transmitter. 
     */
    void begin()
    {
      TIMSK1 |= _BV(OCIE1A);
      m_index = 0;
      m_bit = 0;
      m_sample = 0;
      m_enabled = true;
    }
    
    /**
     * Stop transmitter. 
     */
    void end()
    {
      clear();
      m_enabled = false;
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
     * Send message using a null terminated io vector message. Returns
     * almost immediately, and message will be sent at the right
     * timing by interrupts. Message is gathered from elements in io
     * vector. The total size of the io vector buffers must be less
     * than PAYLOAD_MAX.
     * @param[in] dest destination network address.
     * @param[in] port device port (or message type).
     * @param[in] vec null terminated io vector.
     * @return number of bytes transmitted or negative error code.
     */
    int send(uint8_t dest, uint8_t port, const iovec_t* vec);

    /**
     * Send a message with the given length. Returns almost
     * immediately, and message will be sent at the right timing by
     * interrupts. A command may be given in enhanced mode with
     * addressing to allow identification of the message type.
     * The message length (len) must be less than PAYLOAD_MAX.
     * @param[in] dest destination network address.
     * @param[in] port device port (or message type).
     * @param[in] buf pointer to the data to transmit.
     * @param[in] len number of bytes to transmit.
     * @return number of bytes transmitted or negative error code.
     */
    int send(uint8_t dest, uint8_t port, const void* buf, size_t len);

  private:
    /** Max size of preamble and start symbol. Codec provides actual size */
    static const uint8_t PREAMBLE_MAX = 8;
    
    /** Transmission buffer with premable, start symbol, count and payload */
    uint8_t m_buffer[(MESSAGE_MAX * 2) + PREAMBLE_MAX];

    /** Current transmitter codec */
    Codec* m_codec;

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

    /** Interrupt Service Routine */
    friend void TIMER1_COMPA_vect(void);

    /** Allow access of codec */
    friend class Codec;
  };

private:
  /** Self-reference for interrupt handler */
  static VWI* s_rf;

  /** Receiver member variable */
  Receiver m_rx;

  /** Transmitter member variable */
  Transmitter m_tx;

  /** Bit per second */
  uint16_t m_speed;

  /** Interrupt service routine */
  friend void TIMER1_COMPA_vect(void);
};
#endif
