/**
 * @file RS485.cpp
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

#include "Cosa/Types.h"
#if !defined(BOARD_ATTINY)
#include "RS485.hh"
#include "Cosa/RTT.hh"
#include <util/crc16.h>

static uint8_t
crc7(const void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  uint8_t crc = 0;
  while (size--) {
    uint8_t data = *bp++;
    data ^= crc << 1;
    if (data & 0x80) data ^= 9;
    crc = data ^ (crc & 0x78) ^ (crc << 4) ^ ((crc >> 3) & 0x0f);
  }
  crc = (crc << 1) ^ (crc << 4) ^ (crc & 0x70) ^ ((crc >> 3) & 0x0f);
  return (crc | 1);
}

static uint16_t
crc_xmodem(const void* buf, size_t len)
{
  if (UNLIKELY(len == 0)) return (0);
  uint8_t* bp = (uint8_t*) buf;
  uint16_t crc = 0;
  do crc = _crc_xmodem_update(crc, *bp++); while (--len);
  return (crc);
}

int
RS485::putchar(char c)
{
  // Check if the buffer is full
  while (m_obuf->putchar(c) == IOStream::EOF)
    yield();

  // Enable the transmitter
  *UCSRnB() |= _BV(UDRIE0);
  return (c & 0xff);
}

void
RS485::on_udre_interrupt()
{
  int c = m_obuf->getchar();
  if (c != IOStream::EOF) {
    *UCSRnA() |= _BV(TXC0);
    *UDRn() = c;
  }
  else {
    *UCSRnB() &= ~_BV(UDRIE0);
    *UCSRnB() |= _BV(TXCIE0);
  }
}

void
RS485::on_tx_interrupt()
{
  *UCSRnB() &= ~_BV(TXCIE0);
  on_transmit_completed();
}

int
RS485::send(const void* buf, size_t len, uint8_t dest)
{
  // Check illegal message size, address and state
  if (UNLIKELY(len == 0 || len > PAYLOAD_MAX)) return (EINVAL);
  if (UNLIKELY(dest == m_addr)) return (EINVAL);
  if (UNLIKELY(m_addr != MASTER && dest != MASTER)) return (EINVAL);
  if (UNLIKELY(m_de.is_set())) return (EINVAL);

  // Build message header and calculate payload check-sum
  header_t header;
  header.length = len;
  header.dest = dest;
  header.src = m_addr;
  header.crc = crc7(&header, sizeof(header) - 1);
  uint16_t crc = crc_xmodem(buf, len);

  // Write message; SOT, header, payload and crc
  m_de.set();
  DELAY(100);
  if (putchar(SOT) < 0) return (EIO);
  if (write(&header, sizeof(header)) != (int) sizeof(header)) return (EIO);
  if (write(buf, len) != (int) len) return (EIO);
  if (write(&crc, sizeof(crc)) != sizeof(crc)) return (EIO);
  return (len);
}

int
RS485::recv(void* buf, size_t len, uint32_t ms)
{
  uint32_t start = RTT::millis();
  uint16_t crc;

  // Receive state-machine; start symbol, header, payload and check-sum
  switch (m_state) {
  case 0: // Wait for transmission to complete and start symbol
    while (m_de.is_set()) Power::sleep(SLEEP_MODE_IDLE);
    while (getchar() != SOT) {
      if ((ms != 0) && (RTT::millis() - start > ms)) return (ETIME);
      yield();
    }
    m_state = 1;

  case 1: // Read message header and verify header check-sum
    while (available() != sizeof(header_t)) {
      if ((ms != 0) && (RTT::millis() - start > ms)) return (ETIME);
      yield();
    }
    m_state = 2;
    if (m_ibuf->read(&m_header, sizeof(header_t)) != (int) sizeof(header_t))
      goto error;
    if (m_header.crc != crc7(&m_header, sizeof(header_t) - 1))
      goto error;

  case 2: // Read message payload and verify payload check-sum
    while (available() != (int) (m_header.length + sizeof(crc))) {
      if ((ms != 0) && (RTT::millis() - start > ms)) return (ETIME);
      yield();
    }
    m_state = 3;
  }

  // Check that the given buffer can hold incoming message
  if (m_header.length > len) goto error;
  if (m_ibuf->read(buf, m_header.length) != (int) m_header.length) goto error;
  if (m_ibuf->read(&crc, sizeof(crc)) != sizeof(crc)) goto error;
  if (crc_xmodem(buf, m_header.length) != crc) return (0);
  m_state = 0;

  // Check that the message was addressed to this device
  if ((m_header.dest == m_addr) || (m_header.dest == BROADCAST))
    return (m_header.length);
  return (0);

 error:
  // Something went wrong; flush buffer and signal data error
  m_ibuf->empty();
  m_state = 0;
  return (EFAULT);
}

#define UART_TX_ISR(vec,nr)			\
ISR(vec ## _TX_vect)				\
{						\
  if (UNLIKELY(UART::uart[nr] == NULL)) return;	\
  UART::uart[nr]->on_tx_interrupt();		\
}

#if defined(USART_UDRE_vect)
UART_TX_ISR(USART, 0)
#endif
#if defined(USART1_UDRE_vect)
UART_TX_ISR(USART1, 1)
#endif
#if defined(USART2_UDRE_vect)
UART_TX_ISR(USART2, 2)
#endif
#if defined(USART3_UDRE_vect)
UART_TX_ISR(USART3, 3)
#endif
#endif
