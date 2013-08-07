/**
 * @file Cosa/MPE.hh
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

#ifndef __COSA_MPE_HH__
#define __COSA_MPE_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/PinChangeInterrupt.hh"

/**
 * MPE (Manchester Phase Encoding) is an Cosa library that provides
 * features to send and receive short messages using inexpensive radio
 * transmitters and receivers (RF433) (or other connections).
 */
class MPE {
public:
  /** Max payload size */
  static const uint8_t PAYLOAD_MAX = 32;

private:
  /** CCITT CRC check sum */
  static const uint16_t CRC_CHECK_SUM = 0xf0b8;

  /** Frame size; preamble, framing(count, checksum), payload) */
  static const uint8_t PREAMBLE = 3;
  static const uint8_t FRAMING = 3;
  static const uint8_t FRAME_MAX = PAYLOAD_MAX + FRAMING;

public:
  /**
   * Manchester Phased Encoding Transmitter. 
   */
  class Transmitter : public OutputPin, private Interrupt::Handler {
    friend void TIMER1_COMPA_vect(void);
  private:
    /** Transmitter buffer size; MPE gives two bytes per byte */
    static const uint8_t BUFFER_MAX = PREAMBLE + (FRAME_MAX * 2);

    /** Transmitter buffer */
    uint8_t m_buffer[BUFFER_MAX];

    /** Current encoded message length */
    uint8_t m_length;

    /** Current message byte index */
    uint8_t m_index;

    /** Current bit mask */
    uint8_t m_bit;

    /** Sleep mode on await */
    uint8_t m_mode;

    /** Transmitter status; true(1) when transmitter is active */
    volatile uint8_t m_enabled;

    /** Current transmitter */
    static Transmitter* transmitter;

    /**
     * @override
     * Timer interrupt handler. Send message in transmitter buffer if
     * enabled. 
     */
    virtual void on_interrupt(uint16_t arg = 0);

  public:
    /**
     * Construct Manchester Phased Encoding Transmitter for the given
     * output pin. May be connected to RF433 transmitter or other.
     * @param[in] pin for output.
     */
    Transmitter(Board::DigitalPin pin);

    /**
     * Initiate the transmitter with the given air bit rate and sleep mode
     * on await. Returns true(1) if successful otherwise false(0).
     * @param[in] speed bits per second.
     * @param[in] mode sleep mode when await.
     * @return bool.
     */
    bool begin(uint16_t speed = 4000, uint8_t mode = SLEEP_MODE_IDLE);

    /**
     * Return true(1) if the transmitter is currently active.
     * @return bool.
     */
    bool is_active() { return (m_enabled); }

    /**
     * Wait for the transmitter to complete the current transmission.
     */
    void await();

    /**
     * Send the given message (buffer and number of bytes). Wait for 
     * the transmitter to become available. Return false(0) if the
     * message too large (> PAYLOAD_MAX) otherwise true(1).
     */
    bool send(const void* buf, uint8_t len);
  };

  /**
   * Manchester Phased Encoding Receiver. 
   */
  class Receiver : private PinChangeInterrupt {
  public:
    const uint16_t HIGH_THRESHOLD;
    const uint16_t LOW_THRESHOLD;

    volatile uint8_t m_available;
    volatile uint16_t m_start;
    volatile uint16_t m_samples;
  
    static const uint8_t BUFFER_MAX = FRAME_MAX * 2;
    volatile uint8_t m_buffer[BUFFER_MAX];
    volatile uint8_t m_ix;
    volatile uint8_t m_state;
    volatile uint8_t m_bits;
    volatile uint8_t m_value;
    volatile uint8_t m_count;
  
    /**
     * @override
     * External interrupt handler. Calculates pulse width and maps
     * decodes the Manchester Phased Encoding sequence.
     */
    virtual void on_interrupt(uint16_t arg = 0);

  public:
    /**
     * Create Manchester Phased Encoding Receiver with input pulse
     * sequence from the given pin change interrupt pin and speed,
     * air bits per second.
     * @param[in] pin change interrupt (Default PCI8).
     * @param[in] speed bits per second (Default 4000).
     */
    Receiver(Board::InterruptPin pin = Board::PCI8, uint16_t speed = 4000) : 
      PinChangeInterrupt(pin),
      HIGH_THRESHOLD(speed / 2),
      LOW_THRESHOLD(speed / 20),
      m_available(false),
      m_start(0)
    {
    }

    /**
     * Receive message into given buffer with given size. Wait at most
     * the number of milliseconds (ms). Returns length of received message
     * if successfull, otherwise zero(0) if timeout, or negative error
     * code if failed.
     * @param[in] buf input buffer pointer.
     * @param[in] size of buffer.
     * @param[in] ms timeout period (Default no timeout).
     */
    int recv(void* buf, size_t size, uint32_t ms = 0L);

    /**
     * Start receiver. Allow incoming messages.
     */
    void begin()
    {
      m_start = 0;
      enable();
    }

    /**
     * Stop receiver.
     */
    void end()
    {
      disable();
    }
  
    /**
     * Return number of bytes available.
     */
    uint8_t available() 
    { 
      return (m_available ? m_count : 0); 
    }
  };
};

#endif

