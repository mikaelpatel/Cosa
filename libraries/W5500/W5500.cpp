/**
 * @file W5500.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2017, Mikael Patel, Daniel Sutcliffe
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

#include "W5500.hh"
#include <W5X00.h>
#include <DNS.h>
#include <DHCP.h>

#if !defined(BOARD_ATTINY)

#define M_CREG(name) uint16_t(&m_creg->name), SPI_CP_BSB_CR
#define M_SREG(name) uint16_t(&m_sreg->name), (SPI_CP_BSB_SR | (m_snum<<5))

#define W5X00 W5500
#include <W5X00.inc>


void
W5500::write(uint16_t addr, uint8_t ctl, const void* buf, size_t len, bool progmem)
{
  ctl |= (SPI_CP_RWB_WS | SPI_CP_OM_VDM); // Complete Control Byte by setting to write and variable data length mode

  const uint8_t* bp = (const uint8_t*) buf;
  spi.acquire(this);
    spi.begin();
      spi.transfer_start(addr >> 8);
      spi.transfer_next(addr);
      spi.transfer_next(ctl);
      for (size_t i = 0; i < len; i++, bp++)
	spi.transfer_next(progmem ? pgm_read_byte(bp) : *bp);
      spi.transfer_await();
    spi.end();
  spi.release();
}

void
W5500::read(uint16_t addr, uint8_t ctl, void* buf, size_t len)
{
  ctl |= (SPI_CP_RWB_RS | SPI_CP_OM_VDM); // Complete Control Byte by setting to write and variable data length mode

  uint8_t* bp = (uint8_t*) buf;
  spi.acquire(this);
    spi.begin();
      spi.transfer_start(addr >> 8);
      spi.transfer_next(addr);
      spi.transfer_next(ctl);
      spi.transfer_await();
      for (size_t i = 0; i < len; i++, bp++)
	*bp = spi.transfer(0);
    spi.end();
  spi.release();
}

bool
W5500::begin(uint8_t ip[4], uint8_t subnet[4], uint16_t timeout)
{
  // Initiate socket structure; buffer allocation and socket register pointer
  for (uint8_t i = 0; i < SOCK_MAX; i++) {
    m_sock[i].m_proto = 0;
    m_sock[i].m_snum = i;
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

  // Set source network address, subnet mask and default gateway
  bind(ip, subnet);

  // TODO: Attach interrupt handler
  // spi.attach(this);

  return (true);
}
#endif
