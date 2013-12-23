/**
 * @file Cosa/IOStream/Driver/RS485.hh
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

#ifndef __COSA_IOSTREAM_DRIVER_RS485_HH__
#define __COSA_IOSTREAM_DRIVER_RS485_HH__

#if !defined(__ARDUINO_TINY__)
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Pins.hh"

/**
 * RS485 link handler; Master-Slave protocol. Master always initiates
 * communication with request message to Slave who response. The
 * Master may also broadcast messages to all slave devices. The frame
 * format is; <length,dest,payload,crc> where length is the total
 * length of the frame, dest is the destination device address or the
 * broadcast address(0x00), payload is the message data and crc is a
 * 16 bit CCITT check-sum.
 */
class RS485 : public UART {
public:
  /** Start of transmission token */
  static const uint8_t SOT = 0x01;
  
  /** Size of frame; length(1), dest(1), crc(2) */
  static const uint8_t FRAME_MAX = 4;

  /** Input buffer */
  IOBuffer<UART::BUFFER_MAX> m_ibuf;

  /** Output buffer */
  IOBuffer<UART::BUFFER_MAX> m_obuf;

  /** Data output enable pin; MAX485/DE and /RE */
  OutputPin m_de;

  /** Network address; Special cases are MASTER and BROADCAST */
  uint8_t m_addr;

public:
  /** Max size of payload */
  static const uint8_t PAYLOAD_MAX = BUFFER_MAX - FRAME_MAX - 1;

  /** Network broadcast address */
  static const uint8_t BROADCAST = 0;

  /** Network master address */
  static const uint8_t MASTER = 255;

  /**
   * Construct RS485 network driver with data output enable connect to
   * given pin and given node address.
   * @param[in] port uart port.
   * @param[in] de data output enable.
   * @param[in] addr node address (Default MASTER).
   */
  RS485(uint8_t port, Board::DigitalPin de, uint8_t addr = MASTER) : 
    UART(port, &m_ibuf, &m_obuf),
    m_de(de),
    m_addr(addr)
  {
  }

  /**
   * @override UART
   * Transmit completed callback. Clear data output enable pin.
   */
  virtual void on_transmit_completed() 
  {
    m_de.clear();
  }

  /**
   * Set device address.
   * @param[in] addr device address.
   */
  void set_address(uint8_t addr = MASTER)
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
};
#endif
#endif
