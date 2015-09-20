/**
 * @file CosaRS485.ino
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
 * @section Description
 * Testing the UART transmit completed interrupt handler. First step
 * towards a RS485 driver. Typical usage will have three states; idle,
 * transmitting or receiving.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"

/**
 * RS485 link handler; Master-Slave protocol. Master always initiates
 * communication with request message to Slave who response. The
 * Master may also broadcast messages to all slave devices. The frame
 * format is; <length,dest,payload,crc> where length is the total
 * length of the frame, dest is the destination device address or the
 * broadcast address(0x00), payload is the message data and crc is a
 * 16 bit CCITT check-sum (MSB first/XMODEM).
 */
class RS485 : public UART {
public:
  /** Size of frame; length(1), dest(1), crc(2) */
  static const uint8_t FRAME_MAX = 4;

  /** Input buffer */
  IOBuffer<UART::RX_BUFFER_MAX> m_ibuf;

  /** Output buffer */
  IOBuffer<UART::TX_BUFFER_MAX> m_obuf;

  /** Data output enable pin; MAX485/DE and /RE */
  OutputPin m_de;

  /** Network address; Special cases are MASTER and BROADCAST */
  uint8_t m_addr;

public:
  /** Max size of payload */
  static const uint8_t PAYLOAD_MAX = TX_BUFFER_MAX - FRAME_MAX - 1;

  /** Network broadcast address */
  static const uint8_t BROADCAST = 0;

  /** Network master address */
  static const uint8_t MASTER = 255;

  /**
   * Construct RS485 network driver with data output enable connect to
   * given pin and given node address.
   * @param[in] de data output enable.
   * @param[in] addr node address (Default MASTER).
   */
  RS485(Board::DigitalPin de, uint8_t addr = MASTER) :
    UART(0, &m_ibuf, &m_obuf),
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

#define RS485_DEBUG

#include <util/crc16.h>

static uint16_t
crc_xmodem(const void* buf, size_t len)
{
  if (len == 0) return (0);
  uint8_t* bp = (uint8_t*) buf;
  uint16_t crc = 0;
  do crc = _crc_xmodem_update(crc, *bp++); while (--len);
  return (crc);
}

int
RS485::send(const void* buf, size_t len, uint8_t dest)
{
  // Check illegal message size
  if (len == 0 || len > PAYLOAD_MAX) return (-1);
  // Check no sending to yourself
  if (dest == m_addr) return (-2);
  // Check that slave only sends to master
  if (m_addr != MASTER && dest != MASTER) return (-3);
  // Check that previous send is completed
  if (m_de.is_set()) return (-4);
  // Check for incoming message
  if (m_ibuf.available()) return (-5);
  // Calculate message size and check sum
  uint8_t count = len + FRAME_MAX;
  uint16_t crc = crc_xmodem(buf, len);
  // Write message to output buffer (count, dest, payload, crc)
  m_de.set();
#ifdef RS485_DEBUG
  if (m_ibuf.putchar(count) < 0) return (-3);
  if (m_ibuf.putchar(dest) < 0) return (-3);
  if (m_ibuf.write(buf, len) != (int) len) return (-3);
  if (m_ibuf.write(&crc, sizeof(crc)) != sizeof(crc)) return (-3);
  trace << PSTR("send::len = ") << len << ':'
	<< PSTR("dest = ") << dest << ':'
	<< PSTR("crc = ") << hex << crc << ':'
	<< PSTR("buf = ");
  trace.print(buf, len, IOStream::hex);
#else
  if (putchar(count) < 0) return (-3);
  if (putchar(dest) < 0) return (-3);
  if (write(buf, len) != (int) len) return (-3);
  if (write(&crc, sizeof(crc)) != sizeof(crc)) return (-3);
#endif
  return (len);
}

int
RS485::recv(void* buf, size_t len, uint32_t ms)
{
  // Wait for a message
  if (!available()) {
    uint32_t start = RTC::millis();
    do {
      if ((ms != 0) && (RTC::since(start) > ms)) return (-2);
      Power::sleep();
    } while (!available());
  }
  // Read message from the input buffer (count, dest, payload, crc)
  uint8_t count = getchar() - FRAME_MAX;
  uint8_t dest = getchar();
  uint16_t crc = 0;
  if (count > len) goto error;
  if (m_ibuf.read(buf, count) != count) goto error;
  if (m_ibuf.read(&crc, sizeof(crc)) != sizeof(crc)) goto error;
  if (crc_xmodem(buf, count) != crc) return (0);
#ifdef RS485_DEBUG
  trace << PSTR("recv::len = ") << count << ':'
	<< PSTR("dest = ") << dest << ':'
	<< PSTR("crc = ") << hex << crc << ':'
	<< PSTR("buf = ");
  trace.print(buf, count, IOStream::hex);
#endif
  // Check that the message was addressed to this device
  if ((dest == m_addr) || (dest == BROADCAST)) return (count);
  return (0);

 error:
  // Something went wrong; flush buffer and signal data error
  m_ibuf.empty();
  return (-1);
}

RS485 rs485(Board::LED);
static const uint8_t SLAVE = 0x10;

void setup()
{
  rs485.begin(9600);
  trace.begin(&rs485, PSTR("CosaRS485: started"));
  Watchdog::begin();
  RTC::begin();
  TRACE(free_memory());
  TRACE(sizeof(RS485));
}

void loop()
{
  uint8_t buf[RS485::PAYLOAD_MAX];
  uint8_t msg[] = "GET humitiy";
  uint8_t rep[] = "humidity=50%";

  // MASTER: Too small message
  ASSERT(rs485.send(msg, 0) == -1);
  // MASTER: Too large message
  ASSERT(rs485.send(msg, RS485::PAYLOAD_MAX + 1) == -1);
  // Sending message to oneself
  ASSERT(rs485.send(msg, sizeof(msg)) == -2);
  // MASTER: Broadcast a message
  ASSERT(rs485.broadcast(msg, sizeof(msg)) == sizeof(msg));
  // MASTER: Sending message while still transmitting
  ASSERT(rs485.send(msg, sizeof(msg), SLAVE) == -4);
  sleep(1);

  // SLAVE: Sending message when should receive
  ASSERT(rs485.send(msg, sizeof(msg), SLAVE) == -5);
  // SLAVE: Receive message with too small buffer
  memset(buf, 0, sizeof(buf));
  ASSERT(rs485.recv(buf, 1) == -1);

  // MASTER: Sending message to slave
  ASSERT(rs485.send(msg, sizeof(msg), SLAVE) == sizeof(msg));
  // SLAVE: Receive message
  memset(buf, 0, sizeof(buf));
  ASSERT(rs485.recv(buf, sizeof(buf)) == 0);
  ASSERT(memcmp(buf, msg, sizeof(msg)) == 0);
  sleep(1);

  // SLAVE: Sending message to master
  rs485.set_address(SLAVE);
  ASSERT(rs485.send(rep, sizeof(rep)) == sizeof(rep));
  sleep(1);
  // MASTER: Receive message
  rs485.set_address();
  ASSERT(rs485.recv(buf, sizeof(buf)) == sizeof(rep));
  ASSERT(memcmp(buf, rep, sizeof(rep)) == 0);
  sleep(1);
}
