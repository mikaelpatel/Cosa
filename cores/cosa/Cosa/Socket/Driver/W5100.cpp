/**
 * @file Cosa/Socket/Driver/W5100.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/Socket/Driver/W5100.hh"

#if !defined(__ARDUINO_TINY__)

#define NDEBUG
#ifndef NDEBUG
#include "Cosa/Trace.hh"
#endif

void 
W5100::IRQPin::on_interrupt(uint16_t arg)
{
  if (m_dev == 0) return;
}

W5100::W5100(const uint8_t* mac, Board::DigitalPin csn, Board::ExternalInterruptPin irq) :
  SPI::Driver(csn, 0, SPI::DIV4_CLOCK, 0, SPI::MSB_ORDER, &m_irq),
  m_local(Socket::DYNAMIC_PORT),
  m_mac(mac),
  m_creg((CommonRegister*) COMMON_REGISTER_BASE),
  m_irq(irq, ExternalInterrupt::ON_FALLING_MODE, this)
{
}

void
W5100::write(uint16_t addr, uint8_t data)
{
  spi.begin(this);
  spi.transfer(OP_WRITE);
  spi.transfer(addr >> 8);
  spi.transfer(addr);
  spi.transfer(data);
  spi.end();
#ifndef NDEBUG
  trace << PSTR("write::addr = ") << hex << addr 
	<< PSTR(", data = ") << hex << data 
	<< endl;
#endif
}

void
W5100::write(uint16_t addr, const void* buf, size_t len)
{
  uint16_t last = addr + len;
  spi.begin(this);
  for (const uint8_t* bp = (const uint8_t*) buf; addr < last; bp++, addr++) {
    m_cs.clear();
    spi.transfer(OP_WRITE);
    spi.transfer(addr >> 8);
    spi.transfer(addr);
    spi.transfer(*bp);
    if (addr < last - 1) m_cs.set();
  }
  spi.end();
#ifndef NDEBUG
  addr = addr - len;
  trace << PSTR("write::addr = ") << hex << addr
	<< PSTR(", len = ") << len 
	<< PSTR(", buf = ");
  trace.print(buf, len, IOStream::hex);
#endif
}

void
W5100::write_P(uint16_t addr, const void* buf, size_t len)
{
  uint16_t last = addr + len;
  spi.begin(this);
  for (const uint8_t* bp = (const uint8_t*) buf; addr < last; bp++, addr++) {
    m_cs.clear();
    spi.transfer(OP_WRITE);
    spi.transfer(addr >> 8);
    spi.transfer(addr);
    spi.transfer(pgm_read_byte(bp));
    if (addr < last - 1) m_cs.set();
  }
  spi.end();
#ifndef NDEBUG
  addr = addr - len;
  trace << PSTR("write_P::addr = ") << hex << addr
	<< PSTR(", len = ") << len 
	<< PSTR(", buf = ");
  trace.print(buf, len, IOStream::hex);
#endif
}

uint8_t
W5100::read(uint16_t addr)
{
  spi.begin(this);
  spi.transfer(OP_READ);
  spi.transfer(addr >> 8);
  spi.transfer(addr);
  uint8_t res = spi.transfer(0);
  spi.end();
#ifndef NDEBUG
  trace << PSTR("read::addr = ") << hex << addr 
	<< PSTR(", res = ") << hex << res << endl;
#endif
  return (res);
}

void
W5100::read(uint16_t addr, void* buf, size_t len)
{
  uint16_t last = addr + len;
  spi.begin(this);
  for (uint8_t* bp = (uint8_t*) buf; addr < last; bp++, addr++) {
    m_cs.clear();
    spi.transfer(OP_READ);
    spi.transfer(addr >> 8);
    spi.transfer(addr);
    *bp = spi.transfer(0);
    if (addr < last - 1) m_cs.set();
  }
  spi.end();
#ifndef NDEBUG
  addr = addr - len;
  trace << PSTR("read::addr = ") << hex << addr
	<< PSTR(", len = ") << len
	<< PSTR(", buf = ");
  trace.print(buf, len, IOStream::hex);
#endif
}

void
W5100::issue(uint16_t addr, uint8_t cmd)
{
  write(addr, cmd);
  while (read(addr));
}

int
W5100::Driver::dev_read(void* buf, size_t len)
{
  // Check if there is data available
  int res = available();
  if (res < 0) return (res);

  // If there was no data, check that the socket is still in the correct state
  if (res == 0) {
    uint8_t status = m_dev->read(uint16_t(&m_sreg->SR));
    if ((status == SR_LISTEN) 
	|| (status == SR_CLOSED) 
	|| (status == SR_CLOSE_WAIT))
      return (-3);
    return (0);
  }
  
  // Adjust amount to read to max buffer size
  if (len > res) len = res;

  // Read receiver buffer pointer
  uint16_t ptr;
  m_dev->read(uint16_t(&m_sreg->RX_RD), &ptr, sizeof(ptr));
  ptr = swap((int16_t) ptr);

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
  ptr = swap((int16_t) ptr);
  m_dev->write(uint16_t(&m_sreg->RX_RD), &ptr, sizeof(ptr));
  m_dev->issue(uint16_t(&m_sreg->CR), CR_RECV);
  
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
W5100::Driver::dev_setup()
{
  while (room() < MSG_MAX);
  uint16_t ptr;
  m_dev->read(uint16_t(&m_sreg->TX_WR), &ptr, sizeof(ptr));
  ptr = swap((int16_t) ptr);
  m_tx_offset = ptr & BUF_MASK;
  m_tx_len = 0;
}

int 
W5100::Driver::available() 
{
  // Read receive size register until stable value
  int16_t res, size;
  do {
    m_dev->read(uint16_t(&m_sreg->RX_RSR), &res, sizeof(res));
    m_dev->read(uint16_t(&m_sreg->RX_RSR), &size, sizeof(size));
  } while (res != size);
  return (swap(res));
}

int 
W5100::Driver::room()
{
  // Read transmit free size register until stable value
  int res, size;
  do {
    do {
      m_dev->read(uint16_t(&m_sreg->TX_FSR), &res, sizeof(res));
      m_dev->read(uint16_t(&m_sreg->TX_FSR), &size, sizeof(size));
    } while (res != size);
    res = swap(res);
  } while (res < 0 || res > BUF_MAX);
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
  // Update transmit buffer pointer and issue send command
  uint16_t ptr;
  m_dev->read(uint16_t(&m_sreg->TX_WR), &ptr, sizeof(ptr));
  ptr = swap((int16_t) ptr);
  ptr += m_tx_len;
  ptr = swap((int16_t) ptr);
  m_dev->write(uint16_t(&m_sreg->TX_WR), &ptr, sizeof(ptr));
  m_dev->issue(uint16_t(&m_sreg->CR), CR_SEND);
  uint8_t ir;
  do {
    ir = m_dev->read(uint16_t(&m_sreg->IR));
  } while ((ir & (IR_SEND_OK | IR_TIMEOUT)) == 0);
  m_dev->write(uint16_t(&m_sreg->IR), (IR_SEND_OK | IR_TIMEOUT));
  if (ir & IR_TIMEOUT) return (-1);
  dev_setup();
  return (0);
}

int
W5100::Driver::open(Protocol proto, uint16_t port, uint8_t flag)
{
  // Check if the socket is already in use
  if (m_proto != 0) return (-2);

  // Set protocol and port and issue open command
  m_dev->write(uint16_t(&m_sreg->MR), proto | (flag & MR_FLAG_MASK));
  if (proto == IPRAW)
    m_dev->write(uint16_t(&m_sreg->PROTO), &port, sizeof(m_sreg->PROTO));
  else if ((proto == TCP) || (proto == UDP)) {
    port = swap((int16_t) port);
    m_dev->write(uint16_t(&m_sreg->PORT), &port, sizeof(m_sreg->PORT));
  }
  m_dev->issue(uint16_t(&m_sreg->CR), CR_OPEN);
  
  // Validate status
  uint8_t status = m_dev->read(uint16_t(&m_sreg->SR));
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
  m_dev->issue(uint16_t(&m_sreg->CR), CR_CLOSE);
  m_dev->write(uint16_t(&m_sreg->IR), 0xff);

  // Mark socket as not in use
  m_proto = 0;
  return (0);
}


int 
W5100::Driver::listen()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  m_dev->issue(uint16_t(&m_sreg->CR), CR_LISTEN);
  if (m_dev->read(uint16_t(&m_sreg->SR)) == SR_LISTEN) return (0);
  return (-1);
}

int 
W5100::Driver::accept()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  uint8_t status = m_dev->read(uint16_t(&m_sreg->SR));
  if ((status == SR_LISTEN) || (status == SR_ARP)) return (-3);
  if (status != SR_ESTABLISHED) return (-1);

  // Get connecting client address and setup transmit buffer 
  int16_t dport;
  m_dev->read(uint16_t(&m_sreg->DHAR), m_src.mac, sizeof(m_sreg->DHAR));
  m_dev->read(uint16_t(&m_sreg->DIPR), m_src.ip, sizeof(m_sreg->DIPR));
  m_dev->read(uint16_t(&m_sreg->DPORT), &dport, sizeof(m_sreg->DPORT));
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
  port = swap((int16_t) port);
  m_dev->write(uint16_t(&m_sreg->DIPR), addr, sizeof(m_sreg->DIPR));
  m_dev->write(uint16_t(&m_sreg->DPORT), &port, sizeof(m_sreg->DPORT));
  m_dev->issue(uint16_t(&m_sreg->CR), CR_CONNECT);
  return (0);
}

int 
W5100::Driver::isconnected()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  uint8_t ir = m_dev->read(uint16_t(&m_sreg->IR));
  if (ir & IR_CON) return (1);
  if (ir & IR_TIMEOUT) return (-1);
  dev_setup();
  return (0);
}

int 
W5100::Driver::disconnect()
{
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  m_dev->issue(uint16_t(&m_sreg->CR), CR_DISCON);
  return (0);
}

int 
W5100::Driver::datagram(uint8_t addr[4], uint16_t port)
{
  // Check that the socket is in UDP mode
  if (m_proto != UDP) return (-2);

  // Setup hardware transmit address registers
  port = swap((int16_t) port);
  m_dev->write(uint16_t(&m_sreg->DIPR), addr, sizeof(m_sreg->DIPR));
  m_dev->write(uint16_t(&m_sreg->DPORT), &port, sizeof(port));
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
  if ((m_dev->read(uint16_t(&m_sreg->IR)) & IR_RECV) == 0) return (0);
  int res = dev_read(buf, len);
  return (res);
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
  switch (m_dev->read(uint16_t(&m_sreg->MR)) & MR_PROTO_MASK) {
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
  if (res <= 0) return (res);
  memset(m_src.mac, 0, sizeof(m_src.mac));
  memcpy(m_src.ip, src, sizeof(m_src.ip));
  m_src.port = port;
  return (res);
}

int 
W5100::Driver::write(const void* buf, size_t len, bool progmem)
{
  if ((m_proto == TCP) 
      && (m_dev->read(uint16_t(&m_sreg->SR)) != SR_ESTABLISHED))
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
  // Check that the socket is in TCP mode
  if (m_proto != TCP) return (-2);
  if (m_dev->read(uint16_t(&m_sreg->SR)) != SR_ESTABLISHED) return (-3);
  if (len == 0) return (0);
  if (len > BUF_MAX) len = BUF_MAX;

  // Write data to transmitter buffer
  int res = dev_write(buf, len, progmem);
  if (res <= 0) return (res);

  // Issue send command and wait for completion or timeout
  return (flush() ? -4 : res);
}

int 
W5100::Driver::send(const void* buf, size_t len, 
		    uint8_t dest[4], uint16_t port, 
		    bool progmem)
{
  if ((m_proto != UDP) 
      && (m_proto != IPRAW) 
      && (m_proto != MACRAW)) return (-2);

  // Write parameters (destination address and data), and issue command
  port = swap((int16_t) port);
  m_dev->write(uint16_t(&m_sreg->DIPR), dest, sizeof(m_sreg->DIPR));
  m_dev->write(uint16_t(&m_sreg->DPORT), &port, sizeof(port));

  // Write data to transmitter buffer
  int res = dev_write(buf, len, progmem);
  if (res <= 0) return (res);

  // Issue send command and wait for completion or timeout
  return (flush() ? -4 : res);
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
  timeout = swap((int16_t) timeout * 10);

  // Read hardware address from program memory
  uint8_t mac[6];
  memcpy_P(mac, m_mac, sizeof(mac));

  // Gateway address; assume that the gateway is the first node on local network
  uint8_t gateway[4];
  memcpy(gateway, ip, sizeof(gateway));
  gateway[3] = 1;

  // Reset and setup registers
  write(uint16_t(&m_creg->MR), MR_RST);
  write(uint16_t(&m_creg->SHAR), mac, sizeof(m_creg->SHAR));
  write(uint16_t(&m_creg->SIPR), ip, sizeof(m_creg->SIPR));
  write(uint16_t(&m_creg->SUBR), subnet, sizeof(m_creg->SUBR));
  write(uint16_t(&m_creg->GAR), gateway, sizeof(m_creg->GAR));
  write(uint16_t(&m_creg->RTR), &timeout, sizeof(m_creg->RTR));
  write(uint16_t(&m_creg->TMSR), TX_MEMORY_SIZE);
  write(uint16_t(&m_creg->RMSR), RX_MEMORY_SIZE);

  // Attach interrupt handler 
  spi.attach(this);

  return (true);
}

bool 
W5100::bind(uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4])
{
  // Check for default gateware. Assume router is first addres on network
  uint8_t ROUTER[4];
  if (gateway == NULL) {
    memcpy(ROUTER, ip, sizeof(ROUTER) - 1);
    ROUTER[3] = 1;
    gateway = ROUTER;
  }

  // Write the new network address, subnet mask and gateway address
  write(uint16_t(&m_creg->SIPR), ip, sizeof(m_creg->SIPR));
  write(uint16_t(&m_creg->SUBR), subnet, sizeof(m_creg->SUBR));
  write(uint16_t(&m_creg->GAR), gateway, sizeof(m_creg->GAR));

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

  // Check for dynamic local port allocation
  if (port == 0) {
    port = m_local++;
    if (m_local == 0) m_local = Socket::DYNAMIC_PORT;
  }

  // Open the socket and initiate
  return (sock->open(proto, port, flag) ? NULL : sock);
}
#endif
