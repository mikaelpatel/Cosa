/**
 * @file Cosa/Soft/UART.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_SOFT_UART_HH
#define COSA_SOFT_UART_HH

#include "Cosa/Types.h"
#include "Cosa/OutputPin.hh"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/Serial.hh"
#include "Cosa/IOBuffer.hh"

// Default receiver buffer size
#ifndef COSA_SOFT_UART_RX_BUFFER_MAX
# if defined(BOARD_ATTINY)
#   define COSA_SOFT_UART_RX_BUFFER_MAX 16
# else
#   define COSA_SOFT_UART_RX_BUFFER_MAX 32
# endif
#endif

// Default transmitter buffer size
#ifndef COSA_SOFT_UART_TX_BUFFER_MAX
# if defined(BOARD_ATTINY)
#   define COSA_SOFT_UART_TX_BUFFER_MAX 16
# else
#   define COSA_SOFT_UART_TX_BUFFER_MAX 32
# endif
#endif

namespace Soft {

/**
 * Soft UART for transmission only (UAT) using the OutputPin serial
 * write method. May be used for trace output from ATtiny devices.
 * Has a very small footprint and requires only one pin. No timers.
 */
class UAT : public Serial {
public:
  /**
   * Construct Soft UART with transmitter (only) on given output pin.
   * @param[in] tx transmitter pin.
   */
  UAT(Board::DigitalPin tx);

  /**
   * @override{IOStream::Device}
   * Write character to serial port output buffer. Returns character
   * if successful otherwise on error or buffer full returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * @override{Serial}
   * Start Soft UART device driver (transmitter only).
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default DATA8, NO PARITY, STOP2).
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin(uint32_t baudrate = DEFAULT_BAUDRATE,
		     uint8_t format = DEFAULT_FORMAT);

protected:
  OutputPin m_tx;
  uint8_t m_stops;
  uint8_t m_bits;
  uint16_t m_count;
};

/**
 * Soft UART using serial write and input sampling. The Output serial
 * write method is used for transmission and a pin change interrupt for
 * detecting start condition and receiving data.
 */
class UART : public UAT {
public:
  /** Default buffer size. */
  static const uint8_t RX_BUFFER_MAX = COSA_SOFT_UART_RX_BUFFER_MAX;
  static const uint8_t TX_BUFFER_MAX = COSA_SOFT_UART_TX_BUFFER_MAX;

  /**
   * Construct Soft UART with transmitter on given output pin and
   * receiver on given pin change interrupt pin. Received data is
   * captured by an interrupt service routine and put into given
   * buffer (i.e. io-stream).
   * @param[in] tx transmitter pin.
   * @param[in] rx receiver pin.
   * @param[in] ibuf input buffer.
   */
  UART(Board::DigitalPin tx, Board::InterruptPin rx, IOStream::Device* ibuf);

  /**
   * @override{IOStream::Device}
   * Number of bytes available in input buffer.
   * @return bytes.
   */
  virtual int available()
  {
    return (m_ibuf->available());
  }

  /**
   * @override{IOStream::Device}
   * Peek next character from serial port input buffer.
   * Returns character if successful otherwise on error or buffer empty
   * returns EOF(-1),
   * @return character or EOF(-1).
   */
  virtual int peekchar()
  {
    return (m_ibuf->peekchar());
  }

  /**
   * @override{IOStream::Device}
   * Peek for given character from serial port input buffer.
   * @param[in] c character to peek for.
   * @return available or EOF(-1).
   */
  virtual int peekchar(char c)
  {
    return (m_ibuf->peekchar(c));
  }

  /**
   * @override{IOStream::Device}
   * Read character from serial port input buffer.
   * Returns character if successful otherwise on error or buffer empty
   * returns EOF(-1),
   * @return character or EOF(-1).
   */
  virtual int getchar()
  {
    return (m_ibuf->getchar());
  }

  /**
   * @override{IOStream::Device}
   * Empty internal device buffers.
   */
  virtual void empty()
  {
    m_ibuf->empty();
  }

  /**
   * @override{Serial}
   * Start Soft UART device driver.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default DATA8, NO PARITY, STOP2).
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin(uint32_t baudrate = DEFAULT_BAUDRATE,
		     uint8_t format = DEFAULT_FORMAT);

  /**
   * @override{Serial}
   * Stop Soft UART device driver.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end()
  {
    m_rx.disable();
    return (true);
  }

protected:
  /** Handling start condition and receive data. */
  class RXPinChangeInterrupt : public PinChangeInterrupt {
  public:
    RXPinChangeInterrupt(Board::InterruptPin pin, UART* uart);
    virtual void on_interrupt(uint16_t arg = 0);
  protected:
    UART* m_uart;
  };
  RXPinChangeInterrupt m_rx;
  IOStream::Device* m_ibuf;
  friend class RXPinChangeInterrupt;
};

};

#endif
