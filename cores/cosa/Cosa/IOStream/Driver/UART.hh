/**
 * @file Cosa/IOStream/Driver/UART.hh
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

#ifndef COSA_IOSTREAM_DRIVER_UART_HH
#define COSA_IOSTREAM_DRIVER_UART_HH

// Default reciever buffer size
#ifndef COSA_UART_RX_BUFFER_MAX
# if defined(BOARD_ATTINY)
#   define COSA_UART_RX_BUFFER_MAX 16
# else
#   define COSA_UART_RX_BUFFER_MAX 32
# endif
#endif

// Default transmitter buffer size
#ifndef COSA_UART_TX_BUFFER_MAX
# if defined(BOARD_ATTINY)
#   define COSA_UART_TX_BUFFER_MAX 16
# else
#   define COSA_UART_TX_BUFFER_MAX 32
# endif
#endif

#if defined(BOARD_ATTINY)
#include "Cosa/Soft/UART.hh"
// Default is serial output only
// extern Soft::UART uart;
extern Soft::UAT uart;
#else

#include "Cosa/Types.h"
#include "Cosa/Serial.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Board.hh"

/**
 * Basic UART device handler with internal buffering. IOStream
 * devices may be piped with the IOBuffer class. The UART class
 * requires an input- and output IOBuffer instance.
 */
class UART : public Serial {
public:
  /** Default buffer size for standard UART0 (at 9600 baud). */
  static const uint16_t RX_BUFFER_MAX = COSA_UART_RX_BUFFER_MAX;
  static const uint16_t TX_BUFFER_MAX = COSA_UART_TX_BUFFER_MAX;

  /**
   * Construct serial port handler for given UART.
   * @param[in] port number.
   * @param[in] ibuf input stream buffer.
   * @param[in] obuf output stream buffer.
   */
  UART(uint8_t port, IOStream::Device* ibuf, IOStream::Device* obuf) :
    Serial(),
    m_sfr(Board::UART(port)),
    m_ibuf(ibuf),
    m_obuf(obuf)
  {
    uart[port] = this;
  }

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
   * Number of bytes room in output buffer.
   * @return bytes.
   */
  virtual int room()
  {
    return (m_obuf->room());
  }

  /**
   * @override{IOStream::Device}
   * Write character to serial port output buffer. Returns character
   * if successful otherwise a negative error code.
   * returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

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
   * Flush internal device buffers. Wait for device to become idle.
   * @return zero(0) or negative error code.
   */
  virtual int flush();

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
   * Start UART device driver.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default async, 8data, 2stop bit)
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin(uint32_t baudrate = DEFAULT_BAUDRATE,
		     uint8_t format = DEFAULT_FORMAT);

  /**
   * @override{Serial}
   * Stop UART device driver.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end();

  /**
   * Serial port references. Only uart0 is predefined (reference to global
   * uart). Others are assuped to be allocated by applications and setup
   * with UART_SETUP().
   */
  static UART* uart[Board::UART_MAX];

protected:
  volatile uint8_t* const m_sfr;
  IOStream::Device* m_ibuf;
  IOStream::Device* m_obuf;

  /**
   * Return pointer to UART Control and Status Register A (UCSRnA).
   * @return UCSRAn register pointer.
   */
  volatile uint8_t* UCSRnA() const
  {
    return (m_sfr);
  }

  /**
   * Return pointer to UART Control and Status Register B (UCSRnB).
   * @return UCSRnB register pointer.
   */
  volatile uint8_t* UCSRnB() const
  {
    return (m_sfr + 1);
  }

  /**
   * Return pointer to UART Control and Status Register C (UCSRnC).
   * @return UCSRnC register pointer.
   */
  volatile uint8_t* UCSRnC() const
  {
    return (m_sfr + 2);
  }

  /**
   * Return pointer to UART Baud Rate Register (UBRRn).
   * @return UBRRn register pointer.
   */
  volatile uint16_t* UBRRn() const
  {
    return ((volatile uint16_t*) (m_sfr + 4));
  }

  /**
   * Return pointer to UART I/O Data Register (UDRn).
   * @return UDRn register pointer.
   */
  volatile uint8_t* UDRn() const
  {
    return (m_sfr + 6);
  }

  /**
   * Common UART data register empty (transmit) interrupt handler.
   */
  void on_udre_interrupt();

  /**
   * Common UART receive interrupt handler.
   */
  void on_rx_interrupt();

  /**
   * Common UART transmit completed interrupt handler.
   */
  void on_tx_interrupt();

  /**
   * @override{UART}
   * Transmit completed callback. This virtual member function is
   * called when the last byte in the output buffer is transmitted.
   */
  virtual void on_transmit_completed() {}

#if defined(USART_UDRE_vect)
  friend void USART_UDRE_vect(void);
  friend void USART_RX_vect(void);
  friend void USART_TX_vect(void);
#endif
#if defined(USART1_UDRE_vect)
  friend void USART1_UDRE_vect(void);
  friend void USART1_RX_vect(void);
  friend void USART1_TX_vect(void);
#endif
#if defined(USART2_UDRE_vect)
  friend void USART2_UDRE_vect(void);
  friend void USART2_RX_vect(void);
  friend void USART2_TX_vect(void);
#endif
#if defined(USART3_UDRE_vect)
  friend void USART3_UDRE_vect(void);
  friend void USART3_RX_vect(void);
  friend void USART3_TX_vect(void);
#endif
};

/**
 * Default serial port(0). Weakly defined (See UART.cpp). On Leonardo
 * and other ATmega32u4 based boards the standard serial is CDC.
 */
#if defined(USBCON)
#include "Cosa/IOStream/Driver/CDC.hh"
#define uart cdc
#else
extern UART uart;
#endif
#endif
#endif
