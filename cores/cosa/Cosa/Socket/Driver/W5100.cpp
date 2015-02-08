/**
 * @file Cosa/Socket/Driver/W5100.cpp
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

#include "Cosa/Socket/Driver/W5100.hh"

#if !defined(BOARD_ATTINY)

#include "Cosa/INET/DHCP.hh"
#include "Cosa/INET/DNS.hh"

#define M_CREG(name) uint16_t(&m_creg->name)
#define M_SREG(name) uint16_t(&m_sreg->name)

const uint8_t W5100::MAC[6] __PROGMEM = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

W5100::W5100(const uint8_t* mac, Board::DigitalPin csn) :
  SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0, SPI::MSB_ORDER, NULL),
  m_creg((CommonRegister*) COMMON_REGISTER_BASE),
  m_local(Socket::DYNAMIC_PORT),
  m_mac(mac)
{
  memset(m_dns, 0, sizeof(m_dns));
  if (mac == NULL) m_mac = MAC;
}

void
W5100::write(uint16_t addr, uint8_t data)
{
  spi.acquire(this);
  spi.begin();
  spi.transfer_start(OP_WRITE);
  spi.transfer_next(addr >> 8);
  spi.transfer_next(addr);
  spi.transfer_next(data);
  spi.transfer_await();
  spi.end();
  spi.release();
}

void
W5100::write(uint16_t addr, const void* buf, size_t len)
{
  const uint8_t* bp = (const uint8_t*) buf;
  uint16_t last = addr + len;
  spi.acquire(this);
  spi.begin();
  while (addr < last) {
    spi.transfer_start(OP_WRITE);
    spi.transfer_next(addr >> 8);
    spi.transfer_next(addr++);
    spi.transfer_next(*bp++);
    spi.transfer_await();
    m_cs.set();
    m_cs.clear();
  }
  spi.end();
  spi.release();
}

void
W5100::write_P(uint16_t addr, const void* buf, size_t len)
{
  const uint8_t* bp = (const uint8_t*) buf;
  uint16_t last = addr + len;
  spi.acquire(this);
  spi.begin();
  while (addr < last) {
    spi.transfer_start(OP_WRITE);
    spi.transfer_next(addr >> 8);
    spi.transfer_next(addr++);
    spi.transfer_next(pgm_read_byte(bp++));
    spi.transfer_await();
    m_cs.set();
    m_cs.clear();
  }
  spi.end();
  spi.release();
}

uint8_t
W5100::read(uint16_t addr)
{
  spi.acquire(this);
  spi.begin();
  spi.transfer_start(OP_READ);
  spi.transfer_next(addr >> 8);
  spi.transfer_next(addr);
  spi.transfer_next(0);
  uint8_t res = spi.transfer_await();
  spi.end();
  spi.release();
  return (res);
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

void
W5100::issue(uint16_t addr, uint8_t cmd)
{
  write(addr, cmd);
  do DELAY(10); while (read(addr));
}

int
W5100::Driver::dev_read(void* buf, size_t len)
{
  // Check if there is data available
  int res = available();
  if (res < 0) return (res);

  // Adjust amount to read to max buffer size
  if ((int) len > res) len = res;

  // Read receiver buffer pointer
  uint16_t ptr;
  m_dev->read(M_SREG(RX_RD), &ptr, sizeof(ptr));
  ptr = swap(ptr);

  // Read packet to receiver buffer. Handle possible buffer wrapping
  uint8_t* bp = (uint8_t*) buf;
  uint16_t offset = ptr & BUF_MASK;
  if (offset + len > BUF_MAX) {
    uint16_t size = BUF_MAX - offset;
    m_dev->read(m_rx_buf + offset, bp, size);
    m_dev->read(m_rx_buf, bp + size, len - size);
  }
  else {
    m_dev->read(m_rx_buf + offset, bp, len);
  }

  // Update receiver buffer pointer
  ptr += len;
  ptr = swap(ptr);
  m_dev->write(M_SREG(RX_RD), &ptr, sizeof(ptr));
  m_dev->issue(M_SREG(CR), CR_RECV);

  // Return the number of bytes read
  return (len);
}

int
W5100::Driver::dev_write(const void* buf, size_t len, bool progmem)
{
  // Check buffer size
  if (len == 0) return (0);
  if (len > BUF_MAX) len = BUF_MAX;

  // Write packet to transmitter buffer. Handle possible buffer wrapping
  const uint8_t* bp = (const uint8_t*) buf;
  uint16_t offset = m_tx_offset;
  if (offset + len > BUF_MAX) {
    uint16_t size = BUF_MAX - offset;
    if (progmem) {
      m_dev->write_P(m_tx_buf + offset, bp, size);
      m_dev->write_P(m_tx_buf, bp + size, len - size);
    }
    else {
      m_dev->write(m_tx_buf + offset, bp, size);
      m_dev->write(m_tx_buf, bp + size, len - size);
    }
    m_tx_offset = len - size;
  }
  else {
    if (progmem)
      m_dev->write_P(m_tx_buf + offset, bp, len);
    else
      m_dev->write(m_tx_buf + offset, bp, len);
    m_tx_offset += len;
  }

  // Update transmitter buffer pointer
  m_tx_len += len;

  // Return number of bytes written
  return (len);
}

void
W5100::Driver::dev_flush()
{
  int res = available();
  if (!(res > 0)) return;
  uint16_t ptr;
  m_dev->read(M_SREG(RX_RD), &ptr, sizeof(ptr));
  ptr = swap(ptr);
  ptr += res;
  ptr = swap(ptr);
  m_dev->write(M_SREG(RX_RD), &ptr, sizeof(ptr));
  m_dev->issue(M_SREG(CR), CR_RECV);
}

void
W5100::Driver::dev_setup()
{
  while (room() < (int) MSG_MAX) yield();
  uint16_t ptr;
  m_dev->read(M_SREG(TX_WR), &ptr, sizeof(ptr));
  ptr = swap(ptr);
  m_tx_offset = ptr & BUF_MASK;
  m_tx_len = 0;
}

int
W5100::Driver::available()
{
  // Read receive size register until stable value
  int16_t res, size;
  do {
    m_dev->read(M_SREG(RX_RSR), &res, sizeof(res));
    m_dev->read(M_SREG(RX_RSR), &size, sizeof(size));
  } while (res != size);
  if (res != 0) return (swap(res));
  uint8_t status = m_dev->read(M_SREG(SR));
  if ((status == SR_LISTEN)
      || (status == SR_CLOSED)
      || (status == SR_CLOSE_WAIT))
    return (-1);
  return (0);
}

int
W5100::Driver::room()
{
  // Read transmit free size register until stable value
  int res, size;
  do {
    do {
      m_dev->read(M_SREG(TX_FSR), &res, sizeof(res));
      m_dev->read(M_SREG(TX_FSR), &size, sizeof(size));
    } while (res != size);
    res = swap(res);
  } while (res < 0 || res > (int) BUF_MAX);
  return (res);
}

int
W5100::Driver::read(void* buf, size_t size)
{
  return (dev_read(buf, size));
}

int
W5100::Driver::flush()
{
  // Sanity check status and transmission buffer length
  uint8_t status = m_dev->read(M_SREG(SR));
  if ((status == SR_LISTEN)
      || (status == SR_CLOSED)
      || (status == SR_CLOSE_WAIT))
    return (-1);
  if (m_tx_len == 0) return (0);

  // Update transmit buffer pointer and issue send command
  uint16_t ptr;
  m_dev->read(M_SREG(TX_WR), &ptr, sizeof(ptr));
  ptr = swap(ptr);
  ptr += m_tx_len;
  ptr = swap(ptr);
  m_dev->write(M_SREG(TX_WR), &ptr, sizeof(ptr));
  m_dev->issue(M_SREG(CR), CR_SEND);
  uint8_t ir;
  do {
    ir = m_dev->read(M_SREG(IR));
  } while ((ir & (IR_SEND_OK | IR_TIMEOUT)) == 0);
  m_dev->write(M_SREG(IR), (IR_SEND_OK | IR_TIMEOUT));
  dev_setup();
  if (ir & IR_TIMEOUT) return (-1);
  return (0);
}

int
W5100::Driver::open(Protocol proto, uint16_t port, uint8_t flag)
{
  // Check if the socket is already in use
  if (m_proto != 0) return (-2);

  // Set protocol and port and issue open command
  m_dev->write(M_SREG(MR), proto | (flag & MR_FLAG_MASK));
  if (proto == IPRAW) {
    m_port = port;
    m_dev->write(M_SREG(PROTO), &port, sizeof(m_sreg->PROTO));
  }
  else if ((proto == TCP) || (proto == UDP)) {
    // Check for dynamic local port allocation
    if (port == 0) {
      port = m_dev->m_local++;
      if (m_dev->m_local == 0) m_dev->m_local = Socket::DYNAMIC_PORT;
    }
    m_port = port;
    port = swap(port);
    m_dev->write(M_SREG(PORT), &port, sizeof(m_sreg->PORT));
  }
  m_dev->issue(M_SREG(CR), CR_OPEN);

  // Validate status
  uint8_t status = m_dev->read(M_SREG(SR));
  if (((proto == TCP) && (status != SR_INIT))
      || ((proto == UDP) && (status != SR_UDP))
      || ((proto == IPRAW) && (status != SR_IPRAW))
      || ((proto == MACRAW) && (status != SR_MACRAW))
      || ((proto == PPPoE) && (status != SR_PPPoE)))
    return (-1);

  // Mark socket as in use
  m_proto = proto;
  return (0);
}

int
W5100::Driver::close()
{
  // Check if the socket is not in use
  if (m_proto == 0) return (-2);

  // Issue close command and clear pending interrupts on socket
  m_dev->issue(M_SREG(CR), CR_CLOSE);
  m_dev->write(M_SREG(IR), 0xff);

  // Mark socket as not in use
  m_proto = 0;
  return (0);
}

int
W5100::Driver::listen()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  m_dev->issue(M_SREG(CR), CR_LISTEN);
  if (m_dev->read(M_SREG(SR)) == SR_LISTEN) return (0);
  return (-1);
}

int
W5100::Driver::accept()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  uint8_t status = m_dev->read(M_SREG(SR));
  if ((status == SR_LISTEN) || (status == SR_ARP)) return (-3);
  if (status != SR_ESTABLISHED) return (-1);

  // Get connecting client address and setup transmit buffer
  int16_t dport;
  m_dev->read(M_SREG(DHAR), m_src.mac, sizeof(m_sreg->DHAR));
  m_dev->read(M_SREG(DIPR), m_src.ip, sizeof(m_sreg->DIPR));
  m_dev->read(M_SREG(DPORT), &dport, sizeof(m_sreg->DPORT));
  m_src.port = swap(dport);
  dev_setup();
  return (0);
}

int
W5100::Driver::connect(uint8_t addr[4], uint16_t port)
{
  // Check that the socket is in TCP mode and address/port
  if (m_proto != TCP) return (-2);
  if (INET::is_illegal(addr, port)) return (-1);

  // Set server address and port
  port = swap(port);
  m_dev->write(M_SREG(DIPR), addr, sizeof(m_sreg->DIPR));
  m_dev->write(M_SREG(DPORT), &port, sizeof(m_sreg->DPORT));
  m_dev->issue(M_SREG(CR), CR_CONNECT);
  return (0);
}

int
W5100::Driver::connect(const char* hostname, uint16_t port)
{
  DNS dns;
  if (!dns.begin(m_dev->socket(Socket::UDP), m_dev->m_dns)) return (-3);
  uint8_t dest[4];
  if (dns.gethostbyname(hostname, dest) != 0) return (-4);
  return (connect(dest, port));
}

int
W5100::Driver::is_connected()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  uint8_t ir = m_dev->read(M_SREG(IR));
  if (ir & IR_TIMEOUT) return (-1);
  if ((ir & IR_CON) == 0) return (0);
  dev_setup();
  return (1);
}

int
W5100::Driver::disconnect()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  m_dev->issue(M_SREG(CR), CR_DISCON);
  dev_flush();
  return (0);
}

int
W5100::Driver::datagram(uint8_t addr[4], uint16_t port)
{
  // Check that the socket is in UDP/IPRAW/MACRAW mode
  if ((m_proto != UDP)
      && (m_proto != IPRAW)
      && (m_proto != MACRAW)) return (-2);

  // Setup hardware transmit address registers
  port = swap(port);
  m_dev->write(M_SREG(DIPR), addr, sizeof(m_sreg->DIPR));
  m_dev->write(M_SREG(DPORT), &port, sizeof(port));
  dev_setup();
  return (0);
}

int
W5100::Driver::recv(void* buf, size_t len)
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  if (len == 0) return (0);

  // Check if data has been received
  if ((m_dev->read(M_SREG(IR)) & IR_RECV) == 0) return (0);
  return(dev_read(buf, len));
}

int
W5100::Driver::recv(void* buf, size_t len, uint8_t src[4], uint16_t& port)
{
  if ((m_proto != UDP)
      && (m_proto != IPRAW)
      && (m_proto != MACRAW))
    return (-2);
  if (len == 0) return (0);

  uint8_t header[8];
  uint16_t size;
  int res = -1;

  // Check type of protocol. Read header and data
  switch (m_dev->read(M_SREG(MR)) & MR_PROTO_MASK) {
  case MR_PROTO_UDP:
    res = dev_read(header, 8);
    if (res != 8) return (-1);
    memcpy(src, header, 4);
    port = (header[4] << 8) | header[5];
    size = (header[6] << 8) | header[7];
    if (size > len) size = len;
    res = dev_read(buf, size);
    break;
  case MR_PROTO_IPRAW :
    res = dev_read(header, 6);
    if (res != 6) return (-1);
    memcpy(src, header, 4);
    port = 0;
    size = (header[4] << 8) | header[5];
    if (size > len) size = len;
    res = dev_read(buf, size);
    break;
  case MR_PROTO_MACRAW:
    res = dev_read(header, 2);
    if (res != 2) return (-1);
    memset(src, 0, 4);
    port = 0;
    size = (header[0] << 8) | header[1];
    if (size > len) size = len;
    res = dev_read(buf, size);
  default :
    break;
  }
  if (res > 0) {
    memset(m_src.mac, 0, sizeof(m_src.mac));
    memcpy(m_src.ip, src, sizeof(m_src.ip));
    m_src.port = port;
  }
  return (res);
}

int
W5100::Driver::write(const void* buf, size_t len, bool progmem)
{
  if ((m_proto == TCP)
      && (m_dev->read(M_SREG(SR)) != SR_ESTABLISHED))
    return (-3);
  if (len == 0) return (0);
  const uint8_t* bp = (const uint8_t*) buf;
  int size = len;
  while (size > 0) {
    if (m_tx_len == MSG_MAX) flush();
    int n = MSG_MAX - m_tx_len;
    if (n > size) n = size;
    int res = dev_write(bp, n, progmem);
    if (res < 0) return (res);
    size -= n;
    bp += n;
  }
  return (len);
}

int
W5100::Driver::send(const void* buf, size_t len, bool progmem)
{
  int res = write(buf, len, progmem);
  if (res < 0) return (res);
  return (flush() ? -4 : res);
}

int
W5100::Driver::send(const void* buf, size_t len,
		    uint8_t dest[4], uint16_t port,
		    bool progmem)
{
  if (datagram(dest, port) < 0) return (-1);
  return (send(buf, len, progmem));
}

void
W5100::get_addr(uint8_t ip[4], uint8_t subnet[4])
{
  read(M_CREG(SIPR), ip, sizeof(m_creg->SIPR));
  read(M_CREG(SUBR), subnet, sizeof(m_creg->SUBR));
}

bool
W5100::begin_P(const char* hostname, uint16_t timeout)
{
  // Initiate the socket structures and device
  if (!begin(NULL, NULL, timeout)) return (false);

  // Request a network address from the DHCP server
  DHCP dhcp(hostname, m_mac);
  if (!dhcp.begin(socket(Socket::UDP, DHCP::PORT))) return (false);
  for (uint8_t retry = 0; retry < DNS_RETRY_MAX; retry++) {
    int res = dhcp.discover();
    if (res != 0) continue;
    uint8_t ip[4], subnet[4], gateway[4];
    res = dhcp.request(ip, subnet, gateway);
    if (res != 0) continue;
    bind(ip, subnet, gateway);
    memcpy(m_dns, dhcp.get_dns_addr(), sizeof(m_dns));
    dhcp.end();
    return (true);
  }
  return (false);
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

  // Attach interrupt handler
  // spi.attach(this);

  return (true);
}

int
W5100::bind(uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4])
{
  // Check for default gateway. Assume router is first address on network
  uint8_t ROUTER[4];
  if (gateway == NULL) {
    memcpy(ROUTER, ip, sizeof(ROUTER) - 1);
    ROUTER[3] = 1;
    memcpy(m_dns, ROUTER, sizeof(ROUTER));
    gateway = ROUTER;
  }

  // Write the new network address, subnet mask and gateway address
  write(M_CREG(SIPR), ip, sizeof(m_creg->SIPR));
  write(M_CREG(SUBR), subnet, sizeof(m_creg->SUBR));
  write(M_CREG(GAR), gateway, sizeof(m_creg->GAR));

  return (0);
}

bool
W5100::end()
{
  // Close all sockets and mark as not initiated
  for (uint8_t i = 0; i < SOCK_MAX; i++) m_sock[i].close();
  return (true);
}

Socket*
W5100::socket(Socket::Protocol proto, uint16_t port, uint8_t flag)
{
  // Lookup a free socket
  Driver* sock = NULL;
  for (uint8_t i = 0; i < SOCK_MAX; i++)
    if (m_sock[i].m_proto == 0) {
      sock = &m_sock[i];
      break;
    }
  if (sock == NULL) return (NULL);

  // Open the socket and initiate
  return (sock->open(proto, port, flag) ? NULL : sock);
}
#endif
