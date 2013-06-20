/**
 * @file Cosa/Socket/Driver/NRF24L01P.cpp
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

#include "Cosa/Socket/Driver/NRF24L01P.hh"
#include "Cosa/Watchdog.hh"
#include <util/delay.h>

// Device socket mapping table

int8_t
NRF24L01P::attach(Client* c)
{
  if (m_clients == CLIENT_MAX) return (-1);
  for (uint8_t i = 1; i < CLIENT_MAX; i++) {
    if (m_client[i] == 0) {
      m_client[i] = c;
      m_clients += 1;
      return (i);
    }
  }
  return (-1);
}

void
NRF24L01P::detach(Client* c)
{
  for (uint8_t i = 1; i < CLIENT_MAX; i++) {
    if (m_client[i] != c) continue;
    m_client[i] = 0;
    m_clients -= 1;
    return;
  }
}

// Timing information (ch. 6.1.7, tab. 16, pp. 24)

#define Tpd2stby_ms 3
#define Tstby2a_us 130
#define Thce_us 10

NRF24L01P::NRF24L01P(uint32_t addr,
		     uint8_t channel, 
		     Board::DigitalPin csn, 
		     Board::DigitalPin ce, 
		     Board::ExternalInterruptPin irq) : 
  SPI::Driver(),
  Socket::Device(addr),
  m_csn(csn, 1),
  m_ce(ce, 0),
  m_irq(irq, ExternalInterrupt::ON_FALLING_MODE, this),
  m_state(POWER_DOWN_STATE),
  m_channel(channel),
  m_status(0),
  m_nr_tx(0),
  m_nr_rx(0),
  m_clients(1)
{
  memset(m_client, 0, sizeof(m_client));
}

void
NRF24L01P::set_powerup_mode()
{
  if (m_state != POWER_DOWN_STATE) return;
  m_ce.clear();

  // Setup hardware features, channel, bitrate, retransmission, dynamic payload
  write(FEATURE, _BV(EN_DPL) | _BV(EN_ACK_PAY) | _BV(EN_DYN_ACK));
  write(RF_CH, m_channel);
  write(RF_SETUP, RF_DR_2MBPS | RF_PWR_0DBM);
  write(SETUP_RETR, (2 << ARD) | (15 << ARC));
  write(EN_AA, ENAA_PA);
  write(DYNPD, DPL_PA);

  // Setup hardware receive pipe addresses and enable
  write(SETUP_AW, AW_5BYTES);
  uint8_t rx_addr[AW_MAX];
  set_address(rx_addr, m_addr, DATAGRAM_PIPE);
  write(RX_ADDR_P0, rx_addr, AW_MAX);
  rx_addr[0] = 1;
  write(RX_ADDR_P1, rx_addr, AW_MAX);
  write(RX_ADDR_P2, 2);
  write(RX_ADDR_P3, 3);
  write(RX_ADDR_P4, 4);
  write(RX_ADDR_P5, 5);
  write(EN_RXADDR, ERX_PA); 

  // Setup configuration for powerup and clear interrupts
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP));
  write(STATUS, (_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT)));

  _delay_ms(Tpd2stby_ms);
  m_state = STANDBY_STATE;
}

void
NRF24L01P::set_receiver_mode()
{
  // Fix: Should be handled by the interrupt handler on TX_DS interrupt
  if (m_state == TX_STATE) Watchdog::delay(10);

  // Setup hardware receive pipe address for incoming datagram
  uint8_t rx_addr[AW_MAX];
  set_address(rx_addr, m_addr, DATAGRAM_PIPE);
  write(RX_ADDR_P0, rx_addr, AW_MAX);

  // Setup configruation for primary receiver mode
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP) | _BV(PRIM_RX));
  m_ce.set();
  if (m_state == STANDBY_STATE) _delay_us(Tstby2a_us);
  m_state = RX_STATE;
}

void
NRF24L01P::set_transmitter_mode(uint32_t addr, uint8_t port)
{
  // Handle defaults
  if (addr == 0L) addr = m_addr;

  // Setup hardware transmitter/receiver pipe address for acknowledgement
  uint8_t tx_addr[AW_MAX];
  set_address(tx_addr, addr, port);
  write(TX_ADDR, tx_addr, AW_MAX);
  write(RX_ADDR_P0, tx_addr, AW_MAX);  
  if (m_state == TX_STATE) return;
  m_ce.clear();
  
  // Setup configruation for primary transmitter mode
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP));
  m_ce.set();
  if (m_state == STANDBY_STATE) _delay_us(Tstby2a_us);
  m_state = TX_STATE;
}

void
NRF24L01P::set_standby_mode()
{
  m_ce.clear();
  _delay_us(Thce_us);
  m_state = STANDBY_STATE;
}

void
NRF24L01P::set_powerdown_mode()
{
  m_ce.clear();
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO));
  m_state = POWER_DOWN_STATE;
}

uint8_t 
NRF24L01P::flush()
{
  if (is_max_lost()) {
    write(RF_CH, m_channel);
    m_status = 0xff;
  }
  if (m_state == RX_STATE) {
    asserted(m_csn) {
      spi.exchange(FLUSH_RX);
    }
  } 
  else if (m_state == TX_STATE) {
    asserted(m_csn) {
      spi.exchange(FLUSH_TX);
    }
  }
  write(STATUS, 0xf0);
  return (m_status);
}

void
NRF24L01P::on_event(uint8_t type, uint16_t value)
{
  // Handle receive completed events, filter others
  if (type != Event::RECEIVE_COMPLETED_TYPE) return;

  // Receive the data from the device
  Socket::addr_t src;
  uint16_t dest;
  uint8_t payload[PAYLOAD_MAX];
  int res = recv(dest, payload, sizeof(payload), src);
  if (res <= 0) return;

  // Map to the receiving socket
  Socket* socket = (dest < PIPE_MAX) ? m_client[dest] : lookup(dest);
  if (socket == 0) return;

  // Check if the socket is a connection-less or a client
  if ((!socket->is_server()) && (dest < Socket::DYNAMIC_PORT)) {
    socket->on_recv(payload, res, src);
    set_receiver_mode();
    return;
  }

  // Handle the incoming request/response to server/client
  if (res != sizeof(request_t)) return;
  request_t* request = (request_t*) payload;
  Server* server = (Server*) socket;
  Client* client;
  uint16_t port;
  switch (request->op) {
  case CONNECT_REQUEST:
    if (m_clients == CLIENT_MAX) return;
    client = server->on_connect_request(src);
    if (client == 0) return;
    port = attach(client) + Socket::DYNAMIC_PORT;
    set_port(client, port);
    request->op = CONNECT_RESPONSE;
    send(port, request, sizeof(request_t), src);
    DELAY(200);
    set_receiver_mode();
    break;
  case CONNECT_RESPONSE:
    port = src.port - Socket::DYNAMIC_PORT;
    if (!((port > 0) && (port < CLIENT_MAX))) return;
    dest = dest - Socket::DYNAMIC_PORT;
    if (!((dest > 0) && (dest < CLIENT_MAX))) return;
    client = m_client[dest];
    if (client == 0) return;
    set_connected(client, src);
    client->on_connected();
    break;
  case DISCONNECT_REQUEST:
    // Fix: To be implemented
    break;
  case DISCONNECT_RESPONSE:
    // Fix: To be implemented
    break;
  }
}

int
NRF24L01P::send(uint16_t src, const void* buf, size_t size,
		const Socket::addr_t& dest)
{
  // Check if previous send did not succeed
  if (is_max_retransmit()) {
    write(STATUS, _BV(MAX_RT));
    set_transmitter_mode();
    flush();
  }

  // Check that the transmitter is ready
  else if (!is_ready()) return (0);

  if (size > DATAGRAM_MAX) size = DATAGRAM_MAX;
  set_transmitter_mode(dest.addr, DATAGRAM_PIPE);
  header_t header;
  header.dest.port = dest.port;
  header.src.addr = m_addr;
  header.src.port = src;
  asserted(m_csn) {
    spi.write(W_TX_PAYLOAD, &header, sizeof(header));
    spi.write(buf, size);
  }
  
  // Return size of payload actually sent
  m_nr_tx += 1;
  return (size);
}

int
NRF24L01P::recv(uint16_t& dest, void* buf, size_t size,
		Socket::addr_t& src)
{
  // Check if there is data available on any pipe
  status_t status = get_status();
  uint8_t pipe = status.rx_p_no;
  if (pipe >= PIPE_MAX) return (0);

  // Check if it is a datagram to a connectionless port
  m_nr_rx += 1;
  uint8_t count;
  asserted(m_csn) {
    count = spi.read(R_RX_PL_WID);
  }

  // Check for payload error from device (Tab. 20, pp. 51, R_RX_PL_WID)
  if (count > PAYLOAD_MAX) {
    flush();
    return (-1);
  }

  // Check for datagram handling
  if (pipe == DATAGRAM_PIPE) {
    header_t header;
    count -= sizeof(header);
    if (count > size) count = size;
    asserted(m_csn) {
      m_status = spi.read(R_RX_PAYLOAD, &header, sizeof(header));
      spi.read(buf, count);
    }
    write(STATUS, _BV(RX_DR));
    src = header.src;
    dest = header.dest.port;
    return (count);
  }
  
  // Otherwise the payload is to a connection-oriented port
  if (count > size) count = size;
  asserted(m_csn) {
    m_status = spi.read(R_RX_PAYLOAD, buf, count);
  }
  write(STATUS, _BV(RX_DR));
  dest = pipe;
  return (count);
}

bool 
NRF24L01P::begin(bool mode)
{
  spi.begin(SPI::DIV4_CLOCK, 0, SPI::MSB_FIRST);
  set_powerup_mode();
  if (mode) 
    set_receiver_mode();
  else
    set_transmitter_mode();
  flush();
  enable();
  return (true);
}

bool 
NRF24L01P::end()
{
  set_standby_mode();
  spi.end();
  return (true);
}

bool 
NRF24L01P::available()
{
  status_t status = get_status();
  uint8_t pipe = status.rx_p_no;
  return (pipe >= PIPE_MAX);
}

int
NRF24L01P::send(Socket* s, const void* buf, size_t size, 
		const Socket::addr_t& dest)
{
  return (send(s->get_port(), buf, size, dest));
}

int
NRF24L01P::recv(Socket* s, void* buf, size_t size, 
		Socket::addr_t& src)
{
  uint16_t port = s->get_port();
  status_t status = get_status();
  uint8_t pipe = status.rx_p_no;
  if ((pipe >= PIPE_MAX) || (pipe != port)) return (0);
  return (recv(port, buf, size, src));
}

bool 
NRF24L01P::connect(Client* c, const Socket::addr_t& server)
{
  int8_t pipe = attach(c);
  if (pipe < 0) return (false);
  uint16_t port = pipe + Socket::DYNAMIC_PORT;
  set_port(c, port);
  request_t request;
  request.op = CONNECT_REQUEST;
  int res = send(port, &request, sizeof(request), server);
  set_receiver_mode();
  return (res == sizeof(request));
}

bool 
NRF24L01P::disconnect(Client* c)
{
  // Fix: Should send a disconnect message to the original server.
  // This may require additional address parameter in request message
  // if the server allocated a client handler on another node.
  return (false);
}

int 
NRF24L01P::send(Client* c, const void* buf, size_t size)
{
  // Check that the transmitter is ready
  if (!is_ready()) return (0);

  // Check the payload size
  if (size > PAYLOAD_MAX) size = PAYLOAD_MAX;
  Socket::addr_t dest = get_dest_address(c);
  set_transmitter_mode(dest.addr, dest.port - Socket::DYNAMIC_PORT);
  asserted(m_csn) {
    m_status = spi.write(W_TX_PAYLOAD, buf, size);
  }
  
  // Return size of payload actually sent
  m_nr_tx += 1;
  return (size);
}

int 
NRF24L01P::recv(Client* c, void* buf, size_t size)
{
  uint16_t port = c->get_port();
  if (port < Socket::DYNAMIC_PORT) return (-1);
  port -= Socket::DYNAMIC_PORT;
  status_t status = get_status();
  uint8_t pipe = status.rx_p_no;
  if ((pipe >= PIPE_MAX) || (pipe != port)) return (0);
  Socket::addr_t src;
  return (recv(port, buf, size, src));
}

bool 
NRF24L01P::listen(Server* s)
{
  return (true);
}

void 
NRF24L01P::IRQPin::on_interrupt(uint16_t arg)
{ 
  if (m_nrf == 0) return;
  NRF24L01P::status_t status = m_nrf->get_status();
  m_nrf->write(NRF24L01P::STATUS, (_BV(RX_DR) | _BV(TX_DS)));
  if (!status.rx_dr) return;
  Event::push(Event::RECEIVE_COMPLETED_TYPE, m_nrf);
}

// Output operators for bitfield status registers
IOStream& operator<<(IOStream& outs, NRF24L01P::status_t status)
{
  outs << PSTR("RX_DR = ") << status.rx_dr 
       << PSTR(", TX_DS = ") << status.tx_ds
       << PSTR(", MAX_RT = ") << status.max_rt
       << PSTR(", RX_P_NO = ") << status.rx_p_no
       << PSTR(", TX_FULL = ") << status.tx_full;
  return (outs);
}

IOStream& operator<<(IOStream& outs, NRF24L01P::observe_tx_t observe)
{
  outs << PSTR("PLOS_CNT = ") << observe.plos_cnt
       << PSTR(", ARC_CNT = ") << observe.arc_cnt;
  return (outs);
}

IOStream& operator<<(IOStream& outs, NRF24L01P::fifo_status_t fifo)
{
  outs << PSTR("RX_EMPTY = ") << fifo.rx_empty
       << PSTR(", RX_FULL = ") << fifo.rx_full
       << PSTR(", TX_EMPTY = ") << fifo.tx_empty 
       << PSTR(", TX_FULL = ") << fifo.tx_full 
       << PSTR(", TX_REUSE = ") << fifo.tx_reuse;
  return (outs);
}

