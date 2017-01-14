/**
 * @file W5100.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "W5100.hh"
#include <W5X00.h>
#include <DNS.h>
#include <DHCP.h>

#if !defined(BOARD_ATTINY)

#define M_CREG(name) uint16_t(&m_creg->name)
#define M_SREG(name) uint16_t(&m_sreg->name)

#define W5X00 W5100
#include <W5X00.inc>

void
W5100::write(uint16_t addr, const void* buf, size_t len, bool progmem)
{
  const uint8_t* bp = (const uint8_t*) buf;
  uint16_t last = addr + len;
  spi.acquire(this);
  spi.begin();
  while (addr < last) {
    spi.transfer_start(OP_WRITE);
    spi.transfer_next(addr >> 8);
    spi.transfer_next(addr++);
    spi.transfer_next(progmem ? pgm_read_byte(bp++) : *bp++);
    spi.transfer_await();
    m_cs.set();
    m_cs.clear();
  }
  spi.end();
  spi.release();
}

void
W5100::read(uint16_t addr, void* buf, size_t len)
{
  uint8_t* bp = (uint8_t*) buf;
  uint16_t last = addr + len;
  spi.acquire(this);
  spi.begin();
  while (addr < last) {
    spi.transfer_start(OP_READ);
    spi.transfer_next(addr >> 8);
    spi.transfer_next(addr++);
    spi.transfer_next(0);
    *bp++ = spi.transfer_await();
    m_cs.set();
    m_cs.clear();
  }
  spi.end();
  spi.release();
}

bool
W5100::begin(uint8_t ip[4], uint8_t subnet[4], uint16_t timeout)
{
  // Initiate socket structure; buffer allocation and socket register pointer
  for (uint8_t i = 0; i < SOCK_MAX; i++) {
    SocketRegister* sreg = &((SocketRegister*) SOCKET_REGISTER_BASE)[i];
    m_sock[i].m_proto = 0;
    m_sock[i].m_sreg = sreg;
    m_sock[i].m_tx_buf = TX_MEMORY_BASE + (i * BUF_MAX);
    m_sock[i].m_rx_buf = RX_MEMORY_BASE + (i * BUF_MAX);
    m_sock[i].m_dev = this;
  }

  // Check for default network address
  uint8_t BROADCAST[4] = { 0, 0, 0, 0 };
  if (ip == NULL || subnet == NULL) {
    subnet = BROADCAST;
    ip = BROADCAST;
  }

  // Adjust timeout period to 100 us scale
  timeout = swap(timeout * 10);

  // Read hardware address from program memory
  uint8_t mac[6];
  memcpy_P(mac, m_mac, sizeof(mac));

  // Reset and setup registers
  write(M_CREG(MR), MR_RST);
  write(M_CREG(SHAR), mac, sizeof(m_creg->SHAR));
  write(M_CREG(RTR), &timeout, sizeof(m_creg->RTR));
  write(M_CREG(TMSR), TX_MEMORY_SIZE);
  write(M_CREG(RMSR), RX_MEMORY_SIZE);

  // Set source network address, subnet mask and default gateway
  bind(ip, subnet);

  // TODO: Attach interrupt handler
  // spi.attach(this);

  return (true);
}
#endif
