/**
 * @file CC3000.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "CC3000.hh"
#if !defined(BOARD_ATTINY)
#include "Cosa/RTT.hh"

// Enable trace of unsolicited events
#define TRACE_ON_EVENT
#if defined(TRACE_ON_EVENT)
#include "Cosa/Trace.hh"
#endif

// Some support for setting socket sets for select()
#define FD_ZERO() 0UL
#define FD_ISSET(fd,set) (((1UL << (fd)) & set) != 0UL)
#define FD_SET(fd,set) set = (1UL << (fd)) | set

static void
memrevcpy(void* dest, const void* src, size_t n)
{
  if (UNLIKELY(n == 0)) return;
  uint8_t* dp = (uint8_t*) dest;
  const uint8_t* sp = ((uint8_t*) src) + n ;
  do *dp++ = *--sp; while (--n);
}

void
CC3000::UnsolicitedEvent::on_event(uint16_t event, void* args, size_t len)
{
  UNUSED(len);
#if defined(TRACE_ON_EVENT)
  trace << RTT::millis() << ':';
#endif
  switch (event) {
  case HCI_EVNT_WLAN_UNSOL_KEEPALIVE:
    {
#if defined(TRACE_ON_EVENT)
      trace << PSTR("HCI_EVNT_WLAN_UNSOL_KEEPALIVE:");
#endif
    }
    break;
  case HCI_EVNT_WLAN_UNSOL_CONNECT:
    {
#if defined(TRACE_ON_EVENT)
      trace << PSTR("HCI_EVNT_WLAN_UNSOL_CONNECT:");
#endif
    }
    break;
  case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
    {
#if defined(TRACE_ON_EVENT)
      trace << PSTR("HCI_EVNT_WLAN_UNSOL_DISCONNECT:");
#endif
    }
    break;
  case HCI_EVNT_WLAN_UNSOL_TCP_CLOSE_WAIT:
    {
      hci_evnt_wlan_unsol_tcp_close_wait_t* evnt;
      evnt = (hci_evnt_wlan_unsol_tcp_close_wait_t*) args;
      if (evnt->status != 0) return;
      m_dev->socket_state(evnt->handle, false);
#if defined(TRACE_ON_EVENT)
      trace << PSTR("HCI_EVNT_WLAN_UNSOL_TCP_CLOSE_WAIT:handle=")
	    << evnt->handle << ',';
#endif
    }
    break;
  case HCI_EVNT_DATA_UNSOL_FREE_BUFF:
    {
      hci_evnt_data_unsol_free_buff_t* evnt;
      evnt = (hci_evnt_data_unsol_free_buff_t*) args;
      if (evnt->status != 0) return;
      m_dev->m_buffer_avail += evnt->flow_control_event.buffers_freed;
#if defined(TRACE_ON_EVENT)
      trace << PSTR("HCI_EVNT_DATA_UNSOL_FREE_BUFF:buffers_freed=")
	    << evnt->flow_control_event.buffers_freed << ',';
#endif
    }
    break;
  case HCI_EVNT_WLAN_UNSOL_DHCP:
    {
      hci_evnt_wlan_unsol_dhcp_t* evnt;
      evnt = (hci_evnt_wlan_unsol_dhcp_t*) args;
      if (evnt->status != 0) return;
      memrevcpy(m_dev->m_ip, evnt->ip, sizeof(m_dev->m_ip));
      memrevcpy(m_dev->m_subnet, evnt->subnet, sizeof(m_dev->m_subnet));
      memrevcpy(m_dev->m_gateway, evnt->gateway, sizeof(m_dev->m_gateway));
      memrevcpy(m_dev->m_dns, evnt->dns, sizeof(m_dev->m_dns));
#if defined(TRACE_ON_EVENT)
      trace << PSTR("HCI_EVNT_WLAN_UNSOL_DHCP:");
#endif
    }
    break;
  default:
#if defined(TRACE_ON_EVENT)
    trace << PSTR("HCI_EVNT=") << hex << event
	  << PSTR(",m_buffer_avail=") << m_dev->m_buffer_avail
	  << endl;
    trace.print(args, len, IOStream::hex);
#endif
    return;
  }
#if defined(TRACE_ON_EVENT)
  trace << PSTR("m_buffer_avail=") << m_dev->m_buffer_avail << endl;
#endif
}

int
CC3000::Driver::available()
{
  uint32_t readhndls = FD_ZERO();
  uint32_t writehndls = FD_ZERO();
  uint32_t errorhndls = FD_ZERO();
  FD_SET(m_hndl, readhndls);
  int res = m_dev->select(m_hndl + 1, readhndls, writehndls, errorhndls, 0, 50000);
  if (res < 0) return (res);
  return (FD_ISSET(m_hndl,readhndls));
}

int
CC3000::Driver::room()
{
  // Check room in send buffer
  return (ENOSYS);
}

int
CC3000::Driver::read(void* buf, size_t size)
{
  int res = available();
  if (res < 0) return (IOStream::EOF);
  if (res == 0) return (0);
  return (m_dev->recv(m_hndl, buf, size));
}

int
CC3000::Driver::flush()
{
  // Force send of buffered message
  return (ENOSYS);
}

int
CC3000::Driver::open(Protocol proto, uint16_t port, uint8_t flag)
{
  UNUSED(proto);
  UNUSED(port);
  UNUSED(flag);
  return (ENOSYS);
}

int
CC3000::Driver::close()
{
  return (m_dev->close(m_hndl));
}

int
CC3000::Driver::listen()
{
  return (m_dev->listen(m_hndl));
}

int
CC3000::Driver::accept()
{
  return (m_dev->accept(m_hndl, m_ip, m_port));
}

int
CC3000::Driver::connect(uint8_t addr[4], uint16_t port)
{
  return (m_dev->connect(m_hndl, addr, port));
}

int
CC3000::Driver::connect(const char* hostname, uint16_t port)
{
  UNUSED(hostname);
  UNUSED(port);
  return (ENOSYS);
}

int
CC3000::Driver::is_connected()
{
  return (ENOSYS);
}

int
CC3000::Driver::disconnect()
{
  return (m_dev->close(m_hndl));
}

int
CC3000::Driver::datagram(uint8_t addr[4], uint16_t port)
{
  UNUSED(addr);
  UNUSED(port);
  return (ENOSYS);
}

int
CC3000::Driver::recv(void* buf, size_t len)
{
  return (m_dev->recv(m_hndl, buf, len));
}

int
CC3000::Driver::recv(void* buf, size_t len, uint8_t src[4], uint16_t& port)
{
  UNUSED(buf);
  UNUSED(len);
  UNUSED(src);
  UNUSED(port);
  return (ENOSYS);
}

int
CC3000::Driver::write(const void* buf, size_t size, bool progmem)
{
  UNUSED(buf);
  UNUSED(size);
  UNUSED(progmem);
  return (ENOSYS);
}

int
CC3000::Driver::send(const void* buf, size_t len, bool progmem)
{
  if (progmem) return (ENOSYS);
  return (m_dev->send(m_hndl, buf, len));
}

int
CC3000::Driver::send(const void* buf, size_t len,
		     uint8_t dest[4], uint16_t port,
		     bool progmem)
{
  UNUSED(buf);
  UNUSED(len);
  UNUSED(dest);
  UNUSED(port);
  UNUSED(progmem);
  return (ENOSYS);
}

bool
CC3000::begin_P(str_P hostname, uint16_t timeout)
{
  UNUSED(hostname);
  int res;

  // Setup timeout
  m_timeout = timeout;

  // Startup handshake
  while (m_irq.is_low())
    ;
  m_vbat.high();
  while (m_irq.is_high())
    ;
  DELAY(100);
  m_irq.enable();

  // Start Simple Link
  res = simple_link_start(0);
  if (res < 0) return (false);

  // Set default connection policy
  res = wlan_ioctl_set_connection_policy(false, true, false);
  // res = wlan_ioctl_set_connection_policy(false, false, false);
  if (res < 0) return (false);

  // Read number of buffers and buffer size
  res = read_buffer_size(BUFFER_COUNT, BUFFER_MAX);
  if (res < 0) return (false);
  m_buffer_avail = BUFFER_COUNT;

  // Capture the startup events
  while (wlan_ioctl_statusget() == WLAN_STATUS_CONNECTING)
    service();
  if (res == WLAN_STATUS_CONNECTED) {
    while (*m_ip == 0) service();
  }

  // Read device MAC address
  res = nvmem_read(NVMEM_MAC_FILEID, m_mac, 0, sizeof(m_mac));
  if (res < 0) return (false);
  m_timeout = DEFAULT_TIMEOUT;

  return (true);
}

Socket*
CC3000::socket(Socket::Protocol proto, uint16_t port, uint8_t flag)
{
  UNUSED(port);
  UNUSED(flag);
  int domain = AF_INET;
  int type, protocol;

  switch (proto) {
  case Socket::TCP:
    protocol = IPPROTO_TCP;
    type = SOCK_STREAM;
    break;
  case Socket::UDP:
    protocol = IPPROTO_UDP;
    type = SOCK_DGRAM;
    break;
  case Socket::IPRAW:
    protocol = IPPROTO_RAW;
    type = SOCK_RAW;
    break;
  case Socket::MACRAW:
  case Socket::PPPoE:
  default:
    return (NULL);
  }

  int res = socket(domain, type, protocol);
  if (res < 0) return (NULL);

  Driver* driver = &m_socket[res];
  driver->m_hndl = res;
  driver->m_dev = this;

  return (driver);
}

int
CC3000::service(uint16_t timeout)
{
  uint32_t start = RTT::millis();
  while (1) {
    while (!m_available && (RTT::since(start) < timeout)) yield();
    if (!m_available) return (0);
    await(HCI_EVNT_ANY);
  }
}

bool
CC3000::end()
{
  m_irq.disable();
  m_vbat.low();
  return (true);
}

int
CC3000::wlan_connect(Security type, str_P ssid, str_P bssid, str_P key)
{
  // Check arguments
  if (UNLIKELY(type > WPA2_SECURITY_TYPE)) return (EINVAL);
  if (UNLIKELY(strlen_P(ssid) > HCI_CMND_WLAN_CONNECT_SSID_MAX)) return (EINVAL);
  if (UNLIKELY(strlen_P(key) > HCI_CMND_WLAN_CONNECT_KEY_MAX)) return (EINVAL);

  // Build command block and calculate length
  hci_cmnd_wlan_connect_t cmnd(type, ssid, bssid, key);
  uint8_t len = sizeof(cmnd) - sizeof(cmnd.data);
  len += (cmnd.ssid_length + cmnd.key_length);

  // Issue connect command and await event
  int res = issue(HCI_CMND_WLAN_CONNECT, &cmnd, len);
  if (UNLIKELY(res < 0)) return (res);
  uint32_t saved = m_timeout;
  m_timeout = 10000;
  hci_evnt_wlan_connect_t evnt;
  res = await(HCI_EVNT_WLAN_CONNECT, &evnt, sizeof(evnt));
  m_timeout = saved;
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.result);
}

const CC3000::hci_cmnd_wlan_ioctl_set_scanparam_t
CC3000::DEFAULT_SCANPARAM __PROGMEM = {
  HCI_CMND_WLAN_IOCTL_SET_SCANPARAM_MAGIC, // magic
  4000,					   // intervall
  20,					   // min_dwell_time
  100,					   // max_dwell_time
  5,					   // num_of_probe_requests
  0x1fff,				   // channel_mask
  -120,					   // rssi_threshold
  0,					   // snr_threshold
  300,					   // default_tx_power
  {					   // channel_scan_timeout[16]
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000,
    2000
  }
};

int
CC3000::wlan_ioctl_set_scanparam(const hci_cmnd_wlan_ioctl_set_scanparam_t* param)
{
  int res = issue_P(HCI_CMND_WLAN_IOCTL_SET_SCANPARAM,
		    (param != NULL ? param : &DEFAULT_SCANPARAM),
		    sizeof(hci_cmnd_wlan_ioctl_set_scanparam_t));
  if (UNLIKELY(res < 0)) return (res);

  uint32_t saved = m_timeout;
  m_timeout = 5000;
  hci_evnt_wlan_ioctl_set_scanparam_t evnt;
  res = await(HCI_EVNT_WLAN_IOCTL_SET_SCANPARAM, &evnt, sizeof(evnt));
  m_timeout = saved;
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.result);
}

int
CC3000::wlan_ioctl_statusget()
{
  int res = issue(HCI_CMND_WLAN_IOCTL_STATUSGET);
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_wlan_ioctl_statusget_t evnt;
  res = await(HCI_EVNT_WLAN_IOCTL_STATUSGET, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.wlan_status);
}

int
CC3000::wlan_ioctl_set_connection_policy(bool should_connect_to_open_ap,
					 bool should_use_fast_connect,
					 bool auto_start_use_profiles)
{
  hci_cmnd_wlan_ioctl_set_connection_policy_t
    cmnd(should_connect_to_open_ap, should_use_fast_connect, auto_start_use_profiles);
  int res = issue(HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  uint16_t saved = m_timeout;
  m_timeout = 5000;
  hci_evnt_wlan_ioctl_set_connection_policy_t evnt;
  res = await(HCI_EVNT_WLAN_IOCTL_SET_CONNECTION_POLICY, &evnt, sizeof(evnt));
  m_timeout = saved;
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.result);
}

int
CC3000::wlan_ioctl_get_scan_results(hci_evnt_wlan_ioctl_get_scan_results_t& evnt)
{
  hci_cmnd_wlan_ioctl_get_scan_results_t cmnd(0UL);
  int res = issue(HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  res = await(HCI_EVNT_WLAN_IOCTL_GET_SCAN_RESULTS, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.network_id);
}

int
CC3000::wlan_ioctl_del_profile(uint8_t index)
{
  hci_cmnd_wlan_ioctl_del_profile_t cmnd(index);
  int res = issue(HCI_CMND_WLAN_IOCTL_DEL_PROFILE, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  uint16_t saved = m_timeout;
  m_timeout = 5000;
  hci_evnt_wlan_ioctl_del_profile_t evnt;
  res = await(HCI_EVNT_WLAN_IOCTL_DEL_PROFILE, &evnt, sizeof(evnt));
  m_timeout = saved;
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (0);
}

int
CC3000::wlan_set_event_mask(uint16_t mask)
{
  hci_cmnd_wlan_set_event_mask_t cmnd(mask);
  int res = issue(HCI_CMND_WLAN_SET_EVENT_MASK, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_wlan_set_event_mask_t evnt;
  res = await(HCI_EVNT_WLAN_SET_EVENT_MASK, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (0);
}

int
CC3000::nvmem_read(uint8_t fileid, void* dst, uint32_t src, size_t length)
{
  hci_cmnd_nvmem_read_t cmnd(fileid, src, length);
  int res = issue(HCI_CMND_NVMEM_READ, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_nvmem_read_t evnt;
  res = await(HCI_EVNT_NVMEM_READ, &evnt, sizeof(evnt));
  if (UNLIKELY(res < 0)) return (res);
  delay(10);
  hci_data_nvmem_read_t args;
  res = read_data(HCI_DATA_NVMEM_READ, &args, sizeof(args), dst, length);
  if (UNLIKELY(res < 0)) return (res);
  if ((args.fileid != fileid)
      || (args.length != length)
      || (args.offset != src))
    return (EFAULT);
  return (res);
}

int
CC3000::simple_link_start(uint8_t src)
{
  // Acquire SPI bus and start message transmission after a short delay
  spi.acquire(this);
  spi.begin();
  DELAY(50);

  // SPI header with special delay
  spi.transfer(SPI_OP_WRITE);
  spi.transfer(0);
  spi.transfer(5);
  spi.transfer(0);
  DELAY(50);
  spi.transfer(0);

  // HCI header with start command
  spi.transfer(HCI_TYPE_CMND);
  spi.transfer(HCI_CMND_SIMPLE_LINK_START & 0xff);
  spi.transfer(HCI_CMND_SIMPLE_LINK_START >> 8);
  spi.transfer(1);
  spi.transfer(src);

  spi.end();
  spi.release();

  // Wait for initialization
  delay(1000);
  hci_evnt_simple_link_start_t evnt;
  int res = await(HCI_EVNT_SIMPLE_LINK_START, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.status);
}

int
CC3000::read_buffer_size(uint8_t &count, uint16_t &bytes)
{
  int res = issue(HCI_CMND_READ_BUFFER_SIZE);
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_read_buffer_size_t evnt;
  res = await(HCI_EVNT_READ_BUFFER_SIZE, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  count = evnt.count;
  bytes = evnt.bytes;
  return (evnt.status);
}

int
CC3000::read_sp_version(uint8_t &package_id, uint8_t &package_build_nr)
{
  int res = issue(HCI_CMND_READ_SP_VERSION);
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_read_sp_version_t evnt;
  res = await(HCI_EVNT_READ_SP_VERSION, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  package_id = evnt.package_id;
  package_build_nr = evnt.package_build_nr;
  return (evnt.status);
}

int
CC3000::socket(int domain, int type, int protocol)
{
  if (UNLIKELY(domain != AF_INET)) return (EINVAL);
  hci_cmnd_socket_t cmnd(domain, type, protocol);
  int res = issue(HCI_CMND_SOCKET, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_socket_t evnt;
  res = await(HCI_EVNT_SOCKET, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.handle);
}

int
CC3000::setsockopt(int hndl, int level, int optname, const void* optval, size_t optlen)
{
  hci_cmnd_setsockopt_t cmnd(hndl, level, optname, optval, optlen);
  int res = issue(HCI_CMND_SETSOCKOPT, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_setsockopt_t evnt;
  res = await(HCI_EVNT_SETSOCKOPT, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (0);
}


int
CC3000::select(int hndls,
	       uint32_t &readhndls, uint32_t &writehndls, uint32_t &errorhndls,
	       uint32_t sec, uint32_t us)
{
  hci_cmnd_select_t cmnd(hndls, readhndls, writehndls, errorhndls, sec, us);
  int res = issue(HCI_CMND_SELECT, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_select_t evnt;
  res = await(HCI_EVNT_SELECT, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  readhndls = evnt.read_set;
  writehndls = evnt.write_set;
  errorhndls = evnt.error_set;
  return (evnt.result);
}

int
CC3000::connect(int hndl, uint8_t ip[4], int port)
{
  hci_cmnd_connect_t cmnd(hndl, ip, port);
  int res = issue(HCI_CMND_CONNECT, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_connect_t evnt;
  res = await(HCI_EVNT_CONNECT, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  res = evnt.result;
  if (!socket_state(res, true)) return (EFAULT);
  return (res);
}

int
CC3000::recv(int hndl, void* buf, size_t size)
{
  hci_cmnd_recv_t cmnd(hndl, size);
  int res = issue(HCI_CMND_RECV, &cmnd, sizeof(cmnd));

  hci_evnt_recv_t evnt;
  for (uint8_t retry = 0; retry < 3; retry++) {
    res = await(HCI_EVNT_RECV, &evnt, sizeof(evnt));
    if (res != ENOMSG) break;
  }
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  if (UNLIKELY(evnt.handle != hndl)) return (EFAULT);
  if (UNLIKELY(evnt.count == 0)) return (0);

  uint32_t start = RTT::millis();
  uint32_t TIMEOUT = 3000;
  hci_data_recv_t args;
  do {
    while (!m_available && (RTT::since(start) < TIMEOUT)) yield();
    if (!m_available) return (ETIME);
    res = read_data(HCI_DATA_RECV, &args, sizeof(args), buf, evnt.count);
  } while (res == ENOMSG);
  return (res);
}

int
CC3000::send(int hndl, const void* buf, size_t size)
{
  if (size > BUFFER_MAX) return (EMSGSIZE);
  while (m_buffer_avail == 0) service();
  hci_data_send_t cmnd(hndl, size);
  int res = write_data(HCI_DATA_SEND, &cmnd, sizeof(cmnd), buf, size);
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_send_t evnt;
  res = await(HCI_EVNT_SEND, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(evnt.handle != hndl)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  m_buffer_avail -= 1;
  return (evnt.result);
}

int
CC3000::bind(int hndl, int port)
{
  hci_cmnd_bind_t cmnd(hndl, port);
  int res = issue(HCI_CMND_BIND, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_bind_t evnt;
  res = await(HCI_EVNT_BIND, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.result);
}

int
CC3000::listen(int hndl)
{
  hci_cmnd_listen_t cmnd(hndl);
  int res = issue(HCI_CMND_LISTEN, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_listen_t evnt;
  res = await(HCI_EVNT_LISTEN, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.result);
}

int
CC3000::accept(int hndl, uint8_t ip[4], int &port)
{
  service(1000);

  hci_cmnd_accept_t cmnd(hndl);
  int res = issue(HCI_CMND_ACCEPT, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_accept_t evnt;
  res = await(HCI_EVNT_ACCEPT, &evnt, sizeof(evnt));
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(evnt.result < 0)) res = evnt.result;
  if (UNLIKELY(res < 0)) return (res);
  res = evnt.handle;
  if (!socket_state(res, true)) return (EFAULT);

  memrevcpy(ip, evnt.ip, sizeof(evnt.ip));
  port = evnt.port;
  m_socket[res].m_hndl = res;
  return (res);
}

int
CC3000::close(int hndl)
{
  while (m_buffer_avail != BUFFER_COUNT) service(100);
  service(100);

  hci_cmnd_close_socket_t cmnd(hndl);
  int res = issue(HCI_CMND_CLOSE_SOCKET, &cmnd, sizeof(cmnd));
  if (UNLIKELY(res < 0)) return (res);

  hci_evnt_close_socket_t evnt;
  uint16_t saved = m_timeout;
  m_timeout = 5000;
  res = await(HCI_EVNT_CLOSE_SOCKET, &evnt, sizeof(evnt));
  m_timeout = saved;
  socket_state(hndl, false);
  if (UNLIKELY(evnt.status != 0)) res = EFAULT;
  if (UNLIKELY(res < 0)) return (res);
  return (evnt.result);
}
#endif
