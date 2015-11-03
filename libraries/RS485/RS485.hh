/**
 * @file RS485.hh
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

#ifndef COSA_RS485_HH
#define COSA_RS485_HH

#if !defined(BOARD_ATTINY)
#include "Cosa/UART.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/OutputPin.hh"

// Default buffer size
#ifndef COSA_RS485_BUFFER_MAX
# define COSA_RS485_BUFFER_MAX 64
#endif

/**
 * RS485 link handler; Master-Slave protocol. Master always initiates
 * communication with request message to Slave who responds. The
 * Master may also broadcast messages to all slave devices. The
 * message format is; <header,payload,crc> where header contains the
 * length of the payload, the destination and source device address or
 * the broadcast address(0x00), the source device address and a 8-bit
 * CRC7 check-sum for the header. The payload is the message data and
 * a 16-bit payload CCITT/XMODEM check-sum.
 */
class RS485 : public UART {
public:
  /** Start of transmission token. */
  static const uint8_t SOT = 0x01;

  /** Frame header with check-sum; crc7(1 byte). */
  struct header_t {
    uint8_t length;		//!< Number of bytes in payload.
    uint8_t dest;		//!< Destination node address.
    uint8_t src;		//!< Source node address.
    uint8_t crc;		//!< Header check-sum.
  };

  /** Size of frame; SOT, header and crc. */
  static const uint8_t FRAME_MAX = sizeof(header_t) + sizeof(uint16_t) + 1;

  /** Default buffer size. */
  static const uint16_t BUFFER_MAX = COSA_RS485_BUFFER_MAX;

  /** Network broadcast address. */
  static const uint8_t BROADCAST = 0;

  /** Network master address. */
  static const uint8_t MASTER = 255;

  /**
   * Construct RS485 network driver with data output enable connect to
   * given pin and given node address.
   * @param[in] ibuf input buffer.
   * @param[in] obuf output buffer.
   * @param[in] size of buffers.
   * @param[in] port uart port.
   * @param[in] de data output enable.
   * @param[in] addr node address (Default MASTER).
   */
  RS485(uint8_t port, IOStream::Device* ibuf, IOStream::Device* obuf,
	uint16_t size, Board::DigitalPin de, uint8_t addr = MASTER) :
    UART(port, ibuf, obuf),
    PAYLOAD_MAX(size - FRAME_MAX),
    m_de(de),
    m_addr(addr),
    m_state(0)
  {}

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
   * Set device address.
   * @param[in] addr device address.
   */
  void address(uint8_t addr = MASTER)
  {
    m_addr = addr;
  }

  /**
   * Send message in given buffer and number of bytes to given
   * destination device. Return number of bytes sent or negative error
   * code.
   * @param[in] buf pointer to message buffer.
   * @param[in] len number of bytes.
   * @param[in] dest destination node (Default MASTER).
   * @return number of bytes sent or negative error code.
   */
  int send(const void* buf, size_t len, uint8_t dest = MASTER);

  /**
   * Send message in given buffer and number of bytes to all device on
   * network. Return number of bytes sent or negative error code.
   * @param[in] buf pointer to message buffer.
   * @param[in] len number of bytes.
   * @return number of bytes sent or negative error code.
   */
  int broadcast(const void* buf, size_t len)
    __attribute__((always_inline))
  {
    return (send(buf, len, BROADCAST));
  }

  /**
   * Attempt within given time-limit in milli-seconds receive a message.
   * If received the message is stored in the given buffer with given
   * max length. Returns the number of bytes received or negative
   * error code.
   * @param[in] buf pointer to message buffer.
   * @param[in] len number of bytes.
   * @param[in] ms number of milli-seconds timeout (Default BLOCK).
   * @return number of bytes sent or negative error code.
   */
  int recv(void* buf, size_t len, uint32_t ms = 0L);

protected:
  /** Maximum payload size. */
  const uint16_t PAYLOAD_MAX;

  /** Send/receive header. */
  header_t m_header;

  /** Data output enable pin; MAX485/DE and /RE. */
  OutputPin m_de;

  /** Network address; Special cases are MASTER and BROADCAST. */
  uint8_t m_addr;

  /** Receive state; wait for start symbol, header, payload and check-sum. */
  uint8_t m_state;

  /**
   * @override{UART}
   * RS485 data register empty (transmit) interrupt handler with
   * completion handling.
   */
  virtual void on_udre_interrupt();

  /**
   * @override{UART}
   * RS485 transmit completed interrupt handler. Call extension
   *
   */
  virtual void on_tx_interrupt();

  /**
   * @override{RS485}
   * Transmit completed callback. Clear data output enable pin.
   */
  virtual void on_transmit_completed()
  {
    m_de.clear();
  }
};
#endif
#endif
