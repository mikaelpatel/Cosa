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

#ifndef COSA_CC3000_HH
#define COSA_CC3000_HH

#include "Cosa/Types.h"

#if !defined(BOARD_ATTINY)
#include "HCI.hh"
#include "Cosa/Socket.hh"

/**
 * Cosa TI CC3000 WiFi module device driver class. Provides an
 * implementation of the Cosa Socket and Cosa IOStream::Device
 * classes. A socket may be bound directly to a Cosa IOStream.
 *
 * @section References
 * 1. http://processors.wiki.ti.com/index.php/CC3000
 */
class CC3000 : public HCI {
public:
  /**
   * CC3000 Single-Chip Internet-enable WiFi Driver. Implements the
   * Cosa/Socket interface.
   */
  class Driver : public Socket {
    friend class CC3000;
  public:
    /** Default constructor. */
    Driver() : Socket() {}

    /**
     * @override{IOStream::Device}
     * Returns true if data is available in receiver buffer otherwise false.
     * @return bool.
     */
    virtual int available();

    /**
     * @override{IOStream::Device}
     * Number of bytes room in transmitter buffer before full will be sent.
     * @return bytes.
     */
    virtual int room();

    /** Overloaded virtual member function write. */
    using IOStream::Device::write;

    /**
     * @override{IOStream::Device}
     * Write data from buffer with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const void* buf, size_t size)
    {
      return (write(buf, size, false));
    }

    /**
     * @override{IOStream::Device}
     * Write data from buffer in program memory with given size to device.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write_P(const void* buf, size_t size)
    {
      return (write(buf, size, true));
    }

    /** Overloaded virtual member function read. */
    using IOStream::Device::read;

    /**
     * @override{IOStream::Device}
     * Read data to given buffer with given size from device.
     * @param[in] buf buffer to read into.
     * @param[in] size number of bytes to read.
     * @return number of bytes read or EOF(-1).
     */
    virtual int read(void* buf, size_t size);

    /**
     * @override{IOStream::Device}
     * Flush internal device buffers. Wait for device to become idle.
     * @return zero(0) or negative error code.
     */
    virtual int flush();

    /**
     * @override{Socket}
     * Initiate socket to the given protocol and possible port.
     * @param[in] proto protocol.
     * @param[in] port source port.
     * @param[in] flag socket options.
     * @return zero if successful otherwise negative error code.
     */
    virtual int open(Protocol proto, uint16_t port, uint8_t flag);

    /**
     * @override{Socket}
     * Close the socket.
     * @return zero if successful otherwise negative error code.
     */
    virtual int close();

    /**
     * @override{Socket}
     * Mark socket for incoming requests; server mode.
     * @return zero if successful otherwise negative error code.
     */
    virtual int listen();

    /**
     * @override{Socket}
     * Check for incoming requests from clients. Return zero if
     * the socket has accepted a request and a connection is
     * established.
     * @return zero if successful otherwise negative error code.
     */
    virtual int accept();

    /**
     * @override{Socket}
     * Connect the socket to the given address and port; client mode.
     * @param[in] addr destination address.
     * @param[in] port destination port.
     * @return zero if successful otherwise negative error code.
     */
    virtual int connect(uint8_t addr[4], uint16_t port);

    /**
     * @override{Socket}
     * Connect the socket to the given hostname and port; client mode.
     * Returns zero if connection established otherwise negative error code.
     * @param[in] hostname string.
     * @param[in] port destination port.
     * @return zero if successful otherwise negative error code.
     */
    virtual int connect(const char* hostname, uint16_t port);

    /**
     * @override{Socket}
     * Returns positive integer if a connection is established, zero is
     * not yet established, otherwise a negative error code.
     * @return positive integer connected, zero if not otherwise
     * negative error code.
     */
    virtual int is_connected();

    /**
     * @override{Socket}
     * Disconnect socket from server.
     * @return zero if successful otherwise negative error code.
     */
    virtual int disconnect();

    /**
     * @override{Socket}
     * Start the construction of a datagram to the given address and
     * port.
     * @param[in] addr destination address.
     * @param[in] port destination port.
     * @return zero if successful otherwise negative error code.
     */
    virtual int datagram(uint8_t addr[4], uint16_t port);

    /**
     * @override{Socket}
     * Receive data from connection-oriented socket. The data is stored
     * in given buffer with given maximum number of bytes. Return number of
     * bytes or negative error code.
     * @param[in] buf buffer pointer.
     * @param[in] len number of bytes in buffer.
     * @return number of bytes sent if successful otherwise negative
     * error code.
     */
    virtual int recv(void* buf, size_t len);

    /**
     * @override{Socket}
     * Receive datagram on connectionless socket into given buffer with
     * given maximum size. Returns zero(0) if successful with
     * information in Datagram otherwise negative error code.
     * @param[in] buf buffer pointer.
     * @param[in] len number of bytes in buffer.
     * @param[in] src source address.
     * @param[in] port source port.
     * @return number of bytes received if successful otherwise negative
     * error code.
     */
    virtual int recv(void* buf, size_t len, uint8_t src[4], uint16_t& port);

  protected:
    /**
     * @override{Socket}
     * Write data from buffer with given size to device. Boolean flag
     * progmem defined if the buffer is in program memory. Return number
     * of bytes or negative error code.
     * @param[in] buf buffer to write.
     * @param[in] size number of bytes to write.
     * @param[in] progmem program memory pointer flag.
     * @return number of bytes written or EOF(-1).
     */
    virtual int write(const void* buf, size_t size, bool progmem);

    /**
     * @override{Socket}
     * Send given data in buffer on connection-oriented socket. Boolean flag
     * progmem defined if the buffer is in program memory. Return number
     * of bytes or negative error code.
     * @param[in] buf buffer pointer.
     * @param[in] len number of bytes in buffer.
     * @param[in] progmem program memory pointer flag.
     * @return number of bytes sent if successful otherwise negative
     * error code.
     */
    virtual int send(const void* buf, size_t len, bool progmem);

    /**
     * @override{Socket}
     * Send given data on connectionless socket as a datagram to given
     * destination address (dest:port). Return number of bytes
     * sent or negative error code.
     * @param[in] buf buffer pointer.
     * @param[in] len number of bytes in buffer.
     * @param[in] dest destination address.
     * @param[in] port destination port.
     * @param[in] progmem program memory pointer flag.
     * @return number of bytes sent if successful otherwise negative
     * error code.
     */
    virtual int send(const void* buf, size_t len,
		     uint8_t dest[4], uint16_t port,
		     bool progmem);

    /** Parent device for CC3000 socket driver. */
    CC3000* m_dev;

    /** Socket handle. Should be same as socket index. */
    uint8_t m_hndl;

    /** IP address. */
    uint8_t m_ip[4];

    /** Port. */
    int m_port;
  };

public:
  /**
   * Construct CC3000 device driver with given chip select, interrupt
   * request and enable pin. May also configure with SPI clock rate.
   * @param[in] cs chip select pin.
   * @param[in] irq interrupt request pin.
   * @param[in] vbat enable pin.
   * @param[in] rate of communication with device.
   */
  CC3000(Board::DigitalPin cs,
	 Board::ExternalInterruptPin irq,
	 Board::DigitalPin vbat,
	 SPI::Clock rate = SPI::DEFAULT_CLOCK) :
    HCI(cs, irq, rate),
    m_vbat(vbat, 0),
    m_evnt_handler(this),
    m_active_set(0)
  {
    event_handler(&m_evnt_handler);
  }

  /**
   * Initiate CC3000 device driver with given hostname. Network address,
   * subnet mask and gateway should be obtained from DNS. Returns true
   * if successful otherwise false.
   * @param[in] hostname string in program memory.
   * @param[in] timeout retry timeout period.
   * @return bool.
   */
  bool begin_P(str_P hostname, uint16_t timeout = 5000);

  /**
   * Allocate socket with the given protocol, port and flags. Returns
   * pointer to socket. The socket is deallocated with Socket::close().
   * @param[in] proto socket protocol.
   * @param[in] port number (Default 0).
   * @param[in] flag.
   * @return socket pointer or NULL.
   */
  Socket* socket(Socket::Protocol proto, uint16_t port = 0, uint8_t flag = 0);

  /**
   * Service device events with given timeout.
   * @param[in] timeout in milli-seconds.
   * @return zero or negative error code.
   */
  int service(uint16_t timeout = 100);

  /**
   * Terminate CC3000 device driver. Closes all active sockets. Return
   * true if successful otherwise false.
   */
  bool end();

  /**
   * Get the current network address and subnet mask.
   * @param[in,out] ip network address.
   * @param[in,out] subnet mask.
   */
  void addr(uint8_t ip[4], uint8_t subnet[4])
  {
    memcpy(ip, m_ip, sizeof(m_ip));
    memcpy(subnet, m_subnet, sizeof(m_subnet));
  }

  /**
   * Get the device mac address.
   * @param[in,out] mac device address.
   */
  void mac_addr(uint8_t mac[6])
  {
    memcpy(mac, m_mac, sizeof(m_mac));
  }

  /**
   * Get DNS network address if W5100 device driver was initiated with
   * hostname and obtained network address from DHCP.
   * @param[in,out] ip network address.
   */
  void dns_addr(uint8_t ip[4])
  {
    memcpy(ip, m_dns, sizeof(m_dns));
  }

public:
  /**
   * Handling of unsolicited events from the CC3000 device.
   */
  class UnsolicitedEvent : public HCI::Event::Handler {
  public:
    /**
     * Construct handler for unsolicited events for the given device.
     * @param[in] device.
     */
    UnsolicitedEvent(CC3000* dev) :
      HCI::Event::Handler(),
      m_dev(dev)
    {}

    /*
     * @override{HCI::Event::Handler}
     * Handle unsolicited events; buffer free cound, socket close
     * wait, keepalive, etc.
     * @param[in] event operation code.
     * @param[in] args pointer to argument block.
     * @param[in] len number of bytes in argument block.
     */
    virtual void on_event(uint16_t event, void* args, size_t len);

  protected:
    CC3000* m_dev;
  };

  /**
   * WLAN Security types for wlan_connect().
   */
  enum Security {
    NO_SECURITY_TYPE = 0x0,
    WEP_SECURITY_TYPE = 0x1,
    WPA_SECURITY_TYPE = 0x2,
    WPA2_SECURITY_TYPE = 0x3
  };

  /**
   * Instruct the CC3000 to connect to a given access point.
   * Returns zero if successful otherwise a negative error code.
   * @param[in] type of security.
   * @param[in] ssid string in program memory.
   * @param[in] bssid string in program memory (or NULL).
   * @param[in] key string in program memory.
   * @return zero or negative error code.
   */
  int wlan_connect(Security type, str_P ssid, str_P bssid, str_P key);

  /**
   * Set scan parameters argument block.
   */
  struct hci_cmnd_wlan_ioctl_set_scanparam_t {
    uint32_t magic;
    uint32_t intervall;
    uint32_t min_dwell_time;
    uint32_t max_dwell_time;
    uint32_t num_of_probe_requests;
    uint32_t channel_mask;
    int32_t rssi_threshold;
    uint32_t snr_threshold;
    uint32_t default_tx_power;
    uint32_t channel_scan_timeout[16];
  };

  /**
   * Set scan parameters magic number.
   */
  static const uint32_t HCI_CMND_WLAN_IOCTL_SET_SCANPARAM_MAGIC = 0x00000024L;

  /**
   * Default scan parameters in program memory.
   */
  static const hci_cmnd_wlan_ioctl_set_scanparam_t DEFAULT_SCANPARAM PROGMEM;

  /**
   * Trigger CC3000 to scan available access points with given
   * parameter block in program memory.
   * @param[in] param block.
   * @return zero or negative error code.
   */
  int wlan_ioctl_set_scanparam(const hci_cmnd_wlan_ioctl_set_scanparam_t* param = NULL);

  enum {
    WLAN_STATUS_DISCONNECTED = 0,
    WLAN_STATUS_SCANNING = 1,
    WLAN_STATUS_CONNECTING = 2,
    WLAN_STATUS_CONNECTED = 3
  };

  /**
   * Get WLAN status.
   * @return status or negative error code.
   */
  int wlan_ioctl_statusget();

  /**
   * Set policy to control if and how the CC3000 will try to
   * automatically associate to an access point.
   */
  int wlan_ioctl_set_connection_policy(bool should_connect_to_open_ap,
				       bool should_use_fast_connect,
				       bool auto_start_use_profiles);

  /**
   * Get scan results return block.
   */
  struct hci_evnt_wlan_ioctl_get_scan_results_t {
    int8_t status;
    uint32_t network_id;
    uint32_t scan_status;
    uint8_t valid:1;
    uint8_t rssi:7;
    uint8_t wlan_security:2;
    uint8_t ssid_name_length:6;
    uint16_t entry_time;
    uint8_t ssid[32];
    uint8_t bssid[6];
    uint8_t reserved[2];
  };

  /**
   * Get the results from network scan initiated by calling
   * wlan_ioctl_set_scanparam(). Returns value in event structure. The
   * first call after the scan will have the highest network id and
   * following calls with have network id lower until zero, which
   * indicates the last entry.
   */
  int wlan_ioctl_get_scan_results(hci_evnt_wlan_ioctl_get_scan_results_t& ret);

  /**
   * Delete stored profile.
   * @param[in] index of profile.
   */
  int wlan_ioctl_del_profile(uint8_t index);

  /**
   * Set event mask.
   * @param[in] mask events.
   */
  int wlan_set_event_mask(uint16_t mask);

  enum {
    NVMEM_NVS_FILEID = 0,
    NVMEM_NVS_SHADOW_FILEID = 1,
    NVMEM_WLAN_CONFIG_FILEID = 2,
    NVMEM_WLAN_CONFIG_SHADOW_FILEID = 3,
    NVMEM_WLAN_DRIVER_SP_FILEID = 4,
    NVMEM_WLAN_FW_SP_FILEID = 5,
    NVMEM_MAC_FILEID = 6,
    NVMEM_FRONTEND_VARS_FILEID = 7,
    NVMEM_IP_CONFIG_FILEID = 8,
    NVMEM_IP_CONFIG_SHADOW_FILEID = 9,
    NVMEM_BOOTLOADER_SP_FILEID = 10,
    NVMEM_RM_FILEID = 11,
    NVMEM_AES128_KEY_FILEID = 12,
    NVMEM_SHARED_MEM_FILEID = 13,
    NVMEM_USER_FILE_1_FILEID = 14,
    NVMEM_USER_FILE_2_FILEID = 15,
    NVMEM_MAX_ENTRY = 16
  };

  /**
   * Read CC300 non-volatile memory (EEPROM) block from given source
   * in given file to given destination buffer.
   * @param[in] fileid file identity.
   * @param[in] dst destination buffer.
   * @param[in] src source offset in file.
   * @param[in] n number of bytes to read.
   */
  int nvmem_read(uint8_t fileid, void* dst, uint32_t src, size_t n);

  /**
   * Initialize CC3000 internal modules. Use given patch source (0x00
   * for NVMEM, 0x01 for host, 0x02 for no patches). Returns zero or
   * negative error code.
   * @param[in] src patch source.
   * @return zero or negative error code.
   */
  int simple_link_start(uint8_t src);

  /**
   * Query CC3000 for number of buffers and their size. Returns
   * zero(0) or negative error code.
   * @param[out] count number of buffers.
   * @param[out] bytes per buffer.
   * @return zero or negative error code.
   */
  int read_buffer_size(uint8_t &count, uint16_t &size);

  /**
   * Query CC3000 for service package information; package
   * identification and build number. Returns zero(0) or negative
   * error code.
   * @param[out] package_id.
   * @param[out] package_build_nr.
   * @return zero or negative error code.
   */
  int read_sp_version(uint8_t &package_id, uint8_t &package_build_nr);

  /**
   * Create socket for given domain, type and protocol. Ruturn socket
   * descriptor or negative error code.
   * @param[in] domain (AF_INET).
   * @param[in] type (SOCK_STREAM, SOCK_DGRAM or SOCK_RAW).
   * @param[in] protocol (IPPROTO_TCP, IPPROTO_UDP, IPPROTO_RAW).
   * @return socket descriptor or negative error code.
   */
  int socket(int domain, int type, int protocol);

  enum {
    SOL_SOCKET = 0xffff
  };
  enum {
    SOCKOPT_RECV_NONBLOCK = 0,
    SOCKOPT_RECV_TIMEOUT = 1,
    SOCKOPT_ACCEPT_NONBLOCK = 2
  };

  /**
   * Set socket option. Return zero if successful otherwise a negative
   * error code.
   * @param[in] hndl socket handle.
   * @param[in] level of the option.
   * @param[in] optname option name.
   * @param[in] optval pointer to option block.
   * @param[in] optlen length of option block.
   * @return zero or negative error code.
   */
  int setsockopt(int hndl, int level, int optname, const void* optval, size_t optlen);

  /**
   * Connect to given server with given address and port. Returns zero
   * or negative error code.
   * @param[in] hndl socket descriptor.
   * @param[in] ip address of server.
   * @param[in] port of server.
   * @return zero or negative error code.
   */
  int connect(int hndl, uint8_t ip[4], int port);

  /**
   * Poll given set of handles for waiting read, write or errors operations.
   * Returns zero and socket handle set otherwise a negative error code.
   * @param[in] hndls number of handles in set (max handle number + 1).
   * @param[in,out] readhndls read handle set.
   * @param[in,out] writehndls write handle set.
   * @param[in,out] errorhndls error handle set.
   * @param[in] sec timeout in seconds.
   * @param[in] us timeout in micro-seconds.
   * @return zero or negative error code.
   */
  int select(int hndls,
	     uint32_t &readhnds, uint32_t &writehndls, uint32_t &errorhndls,
	     uint32_t sec = 0UL, uint32_t us = 0UL);

  /**
   * Receive message to given buffer with given size. Returns number
   * of bytes received or negative error code.
   * @param[in] hndl socket descriptor.
   * @param[in] buf message buffer.
   * @param[in] size of message buffer.
   * @return zero or negative error code.
   */
  int recv(int hndl, void* buf, size_t size);

  /**
   * Send message from given buffer with given size. Returns number
   * of bytes sent or negative error code.
   * @param[in] hndl socket descriptor.
   * @param[in] buf message buffer.
   * @param[in] size of message buffer.
   * @return zero or negative error code.
   */
  int send(int hndl, const void* buf, size_t size);

  /**
   * Bind socket to given port and . Returns zero or negative
   * error code.
   * @param[in] hndl socket descriptor.
   * @param[in] port to listen on.
   * @return zero or negative error code.
   */
  int bind(int hndl, int port);

  /**
   * Put socket in listen mode (server). Returns zero or negative
   * error code.
   * @param[in] hndl socket descriptor.
   * @return zero or negative error code.
   */
  int listen(int hndl);

  /**
   * Accept socket connect request. Socket must be in listen
   * mode. Returns socket handle or negative error code.
   * @param[in] hndl socket descriptor.
   * @param[in] ip connecting client address.
   * @param[in] port connect client port.
   * @return socket handle or negative error code.
   */
  int accept(int hndl, uint8_t ip[4], int &port);

  /**
   * Close socket. Returns zero or negative error code.
   * @param[in] hndl socket descriptor.
   * @return zero or negative error code.
   */
  int close(int hndl);

protected:
  /** Function enable pin. */
  OutputPin m_vbat;

  /** Unsolicited Event handler. */
  UnsolicitedEvent m_evnt_handler;

  /** Device mac address. */
  uint8_t m_mac[6];

  /** Network address. */
  uint8_t m_ip[4];

  /** Subnet mask. */
  uint8_t m_subnet[4];

  /** Gateway address. */
  uint8_t m_gateway[4];

  /** DNS server address. */
  uint8_t m_dns[4];

  /** Default startup timeout in ms. */
  static const uint16_t DEFAULT_TIMEOUT = 300;

  /** Size of buffers. Valid after calling begin_P(). */
  uint16_t BUFFER_MAX;

  /** Max number of buffers. Valid after calling begin_P(). */
  uint8_t BUFFER_COUNT;

  /** Current number of buffers. Valid after calling begin_P(). */
  uint8_t m_buffer_avail;

  /** Maximum number of sockets on device. */
  static const int SOCKET_MAX = 8;

  /** Sockets. */
  Driver m_socket[SOCKET_MAX];

  /** Active socket set. */
  uint16_t m_active_set;

  /**
   * Return true if the handle is a possible socket descriptor
   * otherwise false.
   * @param[in] hndl possible socket handle.
   * @return bool.
   */
  bool is_socket(int hndl)
  {
    return (hndl >= 0 && hndl < SOCKET_MAX);
  }

  /**
   * Return true if the handle is an active socket handle
   * otherwise false.
   * @param[in] hndl socket handle.
   * @return bool.
   */
  bool is_active(int hndl)
  {
    if (!is_socket(hndl)) return (false);
    return (bit_get(m_active_set, hndl) != 0);
  }

  /**
   * Mark given socket according to given state. Return true if the
   * handle was a socket descriptor and the operation was successful
   * otherwise false.
   * @param[in] hndl socket handle.
   * @param[in] state of socket (active, deactive).
   * @return bool.
   */
  bool socket_state(int hndl, bool state)
  {
    if (!is_socket(hndl)) return (false);
    bit_write(state, m_active_set, hndl);
    return (true);
  }

  /**
   * HCI Command WLAN connect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_WLAN_CONNECT_.280x0001.29
   */
  static const uint16_t HCI_CMND_WLAN_CONNECT = 0x0001;
  static const uint32_t HCI_CMND_WLAN_CONNECT_MAGIC = 0x0000001cL;
  static const size_t HCI_CMND_WLAN_CONNECT_BSSID_MAX = 6;
  static const size_t HCI_CMND_WLAN_CONNECT_SSID_MAX = 32;
  static const size_t HCI_CMND_WLAN_CONNECT_KEY_MAX = 32;
  static const size_t HCI_CMND_WLAN_CONNECT_DATA_MAX = 64;
  struct hci_cmnd_wlan_connect_t {
    uint32_t magic;
    uint32_t ssid_length;
    uint32_t wlan_security_type;
    uint32_t ssid_length_plus;
    uint32_t key_length;
    uint16_t reserved;
    uint8_t bssid[HCI_CMND_WLAN_CONNECT_BSSID_MAX];
    uint8_t data[HCI_CMND_WLAN_CONNECT_DATA_MAX];
    hci_cmnd_wlan_connect_t(uint8_t type, str_P ssid, str_P bssid, str_P key)
    {
      magic = HCI_CMND_WLAN_CONNECT_MAGIC;
      ssid_length = strlen_P(ssid);
      wlan_security_type = type;
      ssid_length_plus = ssid_length + 16;
      key_length = strlen_P(key);
      reserved = 0;
      if (bssid == NULL)
	memset(this->bssid, 0, sizeof(bssid));
      else
	memcpy_P(this->bssid, bssid, sizeof(bssid));
      memcpy_P(data, ssid, ssid_length);
      memcpy_P(data + ssid_length, key, key_length);
    }
  };

  /**
   * HCI Command WLAN Disconnect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_WLAN_DISCONNECT_.280x0002.29
   */
  static const uint16_t HCI_CMND_WLAN_DISCONNECT = 0x0002;

  /**
   * HCI Command WLAN ioctl set scanparam.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_WLAN_IOCTL_SET_SCANPARAM_.280x0003.29
   */
  static const uint16_t HCI_CMND_WLAN_IOCTL_SET_SCANPARAM = 0x0003;

  /**
   * HCI Command WLAN ioctl set connection policy.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY_.280x0004.29
   */
  static const uint16_t HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY = 0x0004;
  struct hci_cmnd_wlan_ioctl_set_connection_policy_t {
    uint32_t should_connect_to_open_ap;
    uint32_t should_use_fast_connect;
    uint32_t auto_start_use_profiles;
    hci_cmnd_wlan_ioctl_set_connection_policy_t(uint32_t should_connect_to_open_ap,
						uint32_t should_use_fast_connect,
						uint32_t auto_start_use_profiles)
    {
      this->should_connect_to_open_ap = should_connect_to_open_ap;
      this->should_use_fast_connect = should_use_fast_connect;
      this->auto_start_use_profiles = auto_start_use_profiles;
    }
  };

  // Fix: Not yet implemented.
  static const uint16_t HCI_CMND_WLAN_IOCTL_ADD_PROFILE = 0x0005;

  /**
   * HCI Command WLAN ioctl del profile.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_WLAN_IOCTL_DEL_PROFILE_.280x0006.29
   */
  static const uint16_t HCI_CMND_WLAN_IOCTL_DEL_PROFILE = 0x0006;
  struct hci_cmnd_wlan_ioctl_del_profile_t {
    uint32_t index;
    hci_cmnd_wlan_ioctl_del_profile_t(uint8_t index)
    {
      this->index = index;
    }
  };

  /**
   * HCI Command WLAN ioctl get scan results.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS_.280x0007.29
   */
  static const uint16_t HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS = 0x0007;
  struct hci_cmnd_wlan_ioctl_get_scan_results_t {
    uint32_t scan_timeout;
    hci_cmnd_wlan_ioctl_get_scan_results_t(uint32_t scan_timeout = 0UL)
    {
      this->scan_timeout = scan_timeout;
    }
  };

  /**
   * HCI Command WLAN set event mask.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_WLAN_SET_EVENT_MASK_.280x0008.29
   */
  static const uint16_t HCI_CMND_WLAN_SET_EVENT_MASK = 0x0008;
  struct hci_cmnd_wlan_set_event_mask_t {
    uint32_t mask;
    hci_cmnd_wlan_set_event_mask_t(uint16_t mask = 0)
    {
      this->mask = mask;
    }
  };

  // Fix: Not yet implemented.
  static const uint16_t HCI_CMND_WLAN_IOCTL_STATUSGET = 0x0009;
  static const uint16_t HCI_CMND_WLAN_IOCTL_SMART_CONFIG_START = 0x000A;
  static const uint16_t HCI_CMND_WLAN_IOCTL_SMART_CONFIG_STOP = 0x000B;
  static const uint16_t HCI_CMND_WLAN_IOCTL_SMART_CONFIG_SET_PREFIX = 0x000C;

  /**
   * HCI Command NVMEM read.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_NVMEM_READ_.280x0201.29
   */
  static const uint16_t HCI_CMND_NVMEM_READ = 0x0201;
  struct hci_cmnd_nvmem_read_t {
    uint32_t fileid;
    uint32_t length;
    uint32_t offset;
    hci_cmnd_nvmem_read_t(uint8_t fileid, uint32_t offset, size_t length)
    {
      this->fileid = fileid;
      this->length = length;
      this->offset = offset;
    }
  };

  static const uint16_t HCI_CMND_NVMEM_CREATE_ENTRY = 0x0203;

  /**
   * HCI Command read sp version.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_READ_SP_VERSION_.280x0207.29
   */
  static const uint16_t HCI_CMND_READ_SP_VERSION = 0x0207;

  /**
   * HCI Command socket.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_SOCKET_.280x1001.29
   */
  static const uint16_t HCI_CMND_SOCKET = 0x1001;
  struct hci_cmnd_socket_t {
    uint32_t protocol_family;
    uint32_t socket_type;
    uint32_t protocol_type;
    hci_cmnd_socket_t(int domain, int type, int protocol)
    {
      protocol_family = domain;
      socket_type = type;
      protocol_type = protocol;
    }
  };

  /**
   * HCI Command bind.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_BIND_.280x1002.29
   */
  static const uint16_t HCI_CMND_BIND = 0x1002;
  struct hci_cmnd_bind_t {
    uint32_t handle;
    uint32_t reserved;
    uint32_t length;
    uint16_t protocol_family;
    uint16_t port;
    uint8_t ip[4];
    hci_cmnd_bind_t(int hndl, int port)
    {
      handle = hndl;
      reserved = 8;
      length = 8;
      protocol_family = AF_INET;
      this->port = swap(port);
      memset(ip, 0, sizeof(ip));
    }
  };

  /**
   * HCI Command recv.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_RECV_.280x1004.29
   */
  static const uint16_t HCI_CMND_RECV = 0x1004;
  struct hci_cmnd_recv_t {
    uint32_t handle;
    uint32_t count;
    uint32_t flags;
    hci_cmnd_recv_t(int hndl, size_t size)
    {
      handle = hndl;
      count = size;
      flags = 0;
    }
  };

  /**
   * HCI Command accept.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_ACCEPT_.280x1005.29
   */
  static const uint16_t HCI_CMND_ACCEPT = 0x1005;
  struct hci_cmnd_accept_t {
    uint32_t handle;
    hci_cmnd_accept_t(int hndl)
    {
      handle = hndl;
    }
  };

  /**
   * HCI Command listen.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_LISTEN_.280x1006.29
   */
  static const uint16_t HCI_CMND_LISTEN = 0x1006;
  static const uint32_t HCI_CMND_LISTEN_QUEUE_MAX = 4;
  struct hci_cmnd_listen_t {
    uint32_t handle;
    uint32_t queue_depth;
    hci_cmnd_listen_t(int hndl)
    {
      handle = hndl;
      queue_depth = 0UL;
    }
  };

  /**
   * HCI Command connect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_CONNECT_.280x1007.29
   */
  static const uint16_t HCI_CMND_CONNECT = 0x1007;
  struct hci_cmnd_connect_t {
    uint32_t handle;
    uint32_t reserved;
    uint32_t length;
    uint16_t protocol_family;
    uint16_t destination_port;
    uint8_t destination_ip[4];
    hci_cmnd_connect_t(int hndl, uint8_t ip[4], int port)
    {
      handle = hndl;
      reserved = 8;
      length = 8;
      protocol_family = AF_INET;
      destination_port = swap(port);
      memcpy(destination_ip, ip, sizeof(destination_ip));
    }
  };

  /**
   * HCI Command select.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_SELECT_.280x1008.29
   */
  static const uint16_t HCI_CMND_SELECT = 0x1008;
  static const uint32_t HCI_CMND_SELECT_MAGIC = 0x00000014;
  struct hci_cmnd_select_t {
    uint32_t handle_max;
    uint32_t magic[4];
    uint32_t flags;
    uint32_t read_set;
    uint32_t write_set;
    uint32_t error_set;
    struct {
      uint32_t sec;
      uint32_t us;
    } timeout;
    hci_cmnd_select_t(uint32_t hndls,
		      uint32_t readhndls,
		      uint32_t writehndls,
		      uint32_t errorhndls,
		      uint32_t sec,
		      uint32_t us)
    {
      handle_max = hndls;
      for (uint8_t i = 0; i < membersof(magic); i++)
	magic[i] = HCI_CMND_SELECT_MAGIC;
      flags = (sec == 0) && (us == 0);
      read_set = readhndls;
      write_set = writehndls;
      error_set = errorhndls;
      timeout.sec = sec;
      timeout.us = us;
    }
  };

  /**
   * HCI Command set socket option.
   */
  static const uint16_t HCI_CMND_SETSOCKOPT = 0x1009;
  static const uint32_t HCI_CMND_SETSOCKOPT_MAGIC = 0x8;
  enum {
    OPTVAL_MAX = 4
  };
  struct hci_cmnd_setsockopt_t {
    uint32_t handle;
    uint32_t level;
    uint32_t optname;
    uint32_t magic;
    uint32_t optlen;
    uint8_t optval[OPTVAL_MAX];
    hci_cmnd_setsockopt_t(int hndl, int level, int optname,
			  const void* optval, size_t optlen)
    {
      this->handle = hndl;
      this->level = level;
      this->optname = optname;
      this->magic = HCI_CMND_SETSOCKOPT_MAGIC;
      this->optlen = optlen;
      memcpy(this->optval, optval, optlen);
    }
  };

  /**
   * HCI Command get socket option.
   */
  static const uint16_t HCI_CMND_GETSOCKOPT = 0x100A;
  struct hci_cmnd_getsockopt_t {
    uint32_t handle;
    uint32_t level;
    uint32_t optname;
  };

  /**
   * HCI Command close socket.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_CLOSE_SOCKET_.280x100B.29
   */
  static const uint16_t HCI_CMND_CLOSE_SOCKET = 0x100B;
  struct hci_cmnd_close_socket_t {
    uint32_t handle;
    hci_cmnd_close_socket_t(int hndl)
    {
      handle = hndl;
    }
  };

  /**
   * HCI Command get socket option.
   */
  static const uint16_t HCI_CMND_RECVFROM = 0x100D;
  struct hci_cmnd_recvfrom_t { // 12
    uint32_t handle;
  };

  /**
   * HCI Command get host by name.
   */
  static const uint16_t HCI_CMND_GETHOSTBYNAME = 0x100D;
  static const uint16_t HCI_CMND_GETHOSTBYNAME_MAGIC = 8;
  static const uint16_t HOSTNAME_MAX = 32;
  struct hci_cmnd_gethostbyname_t { // 9
    uint32_t magic;
    uint32_t len;
    char hostname[HOSTNAME_MAX];
  };

  // Fix: Not yet implemented
  static const uint16_t HCI_CMND_NETAPP_PING_SEND = 0x2002;
  static const uint16_t HCI_CMND_NETAPP_PING_REPORT = 0x2003;
  static const uint16_t HCI_CMND_NETAPP_PING_STOP = 0x2004;
  static const uint16_t HCI_CMND_NETAPP_GETIPCONFIG = 0x2005;
  static const uint16_t HCI_CMND_NETAPP_ARP_FLUSH = 0x2006;
  static const uint16_t HCI_CMND_NETAPP_SET_DEBUG_LEVEL = 0x2008;

  /**
   * HCI Command Simple Link start.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_SIMPLE_LINK_START_.280x4000.29
   */
  static const uint16_t HCI_CMND_SIMPLE_LINK_START = 0x4000;
  struct hci_cmnd_simple_link_start_t {
    uint8_t src;
  };

  /**
   * HCI Command read buffer size.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_CMND_messages#HCI_CMND_READ_BUFFER_SIZE_.280x400B.29
   */
  static const uint16_t HCI_CMND_READ_BUFFER_SIZE = 0x400B;

  /**
   * HCI Data Command send.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_DATA_messages#HCI_DATA_SEND_.280x81.29
   */
  static const uint8_t HCI_DATA_SEND = 0x81;
  static const uint32_t HCI_DATA_SEND_MAGIC = 0x0000000C;
  struct hci_data_send_t {
    uint32_t handle;
    uint32_t magic;
    uint32_t tcp_data_length;
    uint32_t flags;
    hci_data_send_t(int hndl, size_t size)
    {
      handle = hndl;
      magic = HCI_DATA_SEND_MAGIC;
      tcp_data_length = size;
      flags = 0;
    }
  };

  // Fix: Not yet implemented.
  static const uint8_t HCI_DATA_SENDTO = 0x83;

  /**
   * HCI Data Command recv.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_DATA_messages
   */
  static const uint8_t HCI_DATA_RECV = 0x85;
  struct hci_data_recv_t {
    uint32_t handle;
    uint32_t magic;
    uint32_t tcp_data_length;
    uint32_t flags;
    uint32_t reserved[2];
  };

  // Fix: Not yet implemented.
  static const uint8_t HCI_DATA_NVMEM_WRITE = 0x90;

  /**
   * HCI Data Command recv.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_DATA_messages#HCI_DATA_NVMEM_READ_.280x91.29
   */
  static const uint8_t HCI_DATA_NVMEM_READ = 0x91;
  struct hci_data_nvmem_read_t {
    uint32_t fileid;
    uint32_t length;
    uint32_t offset;
    uint32_t reserved[3];
  };

  /** HCI Synthetic Event for unsolicited event service. */
  static const uint16_t HCI_EVNT_ANY = 0x0000;

  /**
   * HCI Event WLAN connect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_CONNECT_.280x0001.29
   */
  static const uint16_t HCI_EVNT_WLAN_CONNECT = 0x0001;
  struct hci_evnt_wlan_connect_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event WLAN disconnect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_DISCONNECT_.280x0002.29
   */
  static const uint16_t HCI_EVNT_WLAN_DISCONNECT = 0x0002;
  struct hci_evnt_wlan_disconnect_t {
    int8_t status;
    uint32_t result;
  };

  /**
   * HCI Event WLAN ioctl set scanparam
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_IOCTL_SET_SCANPARAM_.280x0003.29
   */
  static const uint16_t HCI_EVNT_WLAN_IOCTL_SET_SCANPARAM = 0x0003;
  struct hci_evnt_wlan_ioctl_set_scanparam_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event WLAN ioctl set connection policy
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_WLAN_IOCTL_SET_CONNECTION_POLICY = 0x0004;
  struct hci_evnt_wlan_ioctl_set_connection_policy_t {
    int8_t status;
    int32_t result;
  };

  // Fix: Not yet implemented.
  static const uint16_t HCI_EVNT_WLAN_IOCTL_ADD_PROFILE = 0x0005;

  /**
   * HCI Event WLAN ioctl del profile
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_WLAN_IOCTL_DEL_PROFILE = 0x0006;
  struct hci_evnt_wlan_ioctl_del_profile_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event WLAN ioctl set scanparam
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_IOCTL_SET_SCANPARAM_.280x0003.29
   */
  static const uint16_t HCI_EVNT_WLAN_IOCTL_GET_SCAN_RESULTS = 0x0007;

  /**
   * HCI Event WLAN set event mask
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_WLAN_SET_EVENT_MASK = 0x0008;
  struct hci_evnt_wlan_set_event_mask_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event WLAN ioctl set scanparam
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_IOCTL_SET_SCANPARAM_.280x0003.29
   */
  static const uint16_t HCI_EVNT_WLAN_IOCTL_STATUSGET = 0x0009;
  struct hci_evnt_wlan_ioctl_statusget_t {
    int8_t status;
    uint32_t wlan_status;
  };

  // Fix: Not yet implemented.
  static const uint16_t HCI_EVNT_WLAN_IOCTL_SMART_CONFIG_SET_PREFIX = 0x000C;

  /**
   * HCI Event WLAN ioctl set scanparam
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_IOCTL_SET_SCANPARAM_.280x0003.29
   */
  static const uint16_t HCI_EVNT_NVMEM_READ = 0x0201;
  struct hci_evnt_nvmem_read_t {
    int8_t status;
  };

  /**
   * HCI Event read sp version.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_READ_SP_VERSION_.280x0207.29
   */
  static const uint16_t HCI_EVNT_READ_SP_VERSION = 0x0207;
  struct hci_evnt_read_sp_version_t {
    int8_t status;
    uint16_t dummy;
    uint8_t package_id;
    uint8_t package_build_nr;
  };

  static const uint16_t HCI_EVNT_PATCHES_REQ = 0x1000;

  /**
   * HCI Event socket.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_SOCKET_.280x1001.29
   */
  static const uint16_t HCI_EVNT_SOCKET = 0x1001;
  struct hci_evnt_socket_t {
    int8_t status;
    int32_t handle;
  };

  /**
   * HCI Event bind.
   *
   */
  static const uint16_t HCI_EVNT_BIND = 0x1002;
  struct hci_evnt_bind_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event send.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_DATA_SENDTO_.280x100F.29
   */
  static const uint16_t HCI_EVNT_SEND = 0x1003;
  struct hci_evnt_send_t {
    int8_t status;
    int32_t handle;
    int32_t result;
  };

  /**
   * HCI Event recv.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_RECV = 0x1004;
  struct hci_evnt_recv_t {
    int8_t status;
    int32_t handle;
    int32_t count;
    int32_t result;
  };

  /**
   * HCI Event accept.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_ACCEPT = 0x1005;
  struct hci_evnt_accept_t {
    int8_t status;
    int32_t result;
    int32_t handle;
    uint16_t protocol_family;
    uint16_t port;
    uint8_t ip[4];
  };

  /**
   * HCI Event listen.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_LISTEN = 0x1006;
  struct hci_evnt_listen_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event connect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_CONNECT_.280x1007.29
   */
  static const uint16_t HCI_EVNT_CONNECT = 0x1007;
  struct hci_evnt_connect_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event select.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_SELECT = 0x1008;
  struct hci_evnt_select_t {
    int8_t status;
    int32_t result;
    uint32_t read_set;
    uint32_t write_set;
    uint32_t error_set;
  };

  /**
   * HCI Command set socket option.
   */
  static const uint16_t HCI_EVNT_SETSOCKOPT = 0x1009;
  struct hci_evnt_setsockopt_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event close socket.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages
   */
  static const uint16_t HCI_EVNT_CLOSE_SOCKET = 0x100B;
  struct hci_evnt_close_socket_t {
    int8_t status;
    int32_t result;
  };

  /**
   * HCI Event sendto.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_DATA_SENDTO_.280x100F.29
   */
  static const uint16_t HCI_EVNT_DATA_SENDTO = 0x100F;
  struct hci_evnt_sendto_t {
    int8_t status;
    int32_t handle;
    int32_t result;
  };

  /**
   * HCI Event NetApp ping send.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_NETAPP_PING_SEND_.280x2002.29
   */
  static const uint16_t HCI_EVNT_NETAPP_PING_SEND = 0x2002;
  struct hci_evnt_netapp_ping_send_t {
    int8_t status;
  };

  /**
   * HCI Event NetApp ping report.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_NETAPP_PING_REPORT_.280x2003.29
   */
  static const uint16_t HCI_EVNT_NETAPP_PING_REPORT = 0x2003;
  struct hci_evnt_netapp_ping_report_t {
    int8_t status;
  };

  /**
   * HCI Event NetApp ping stop.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_NETAPP_PING_STOP_.280x2004.29
   */
  static const uint16_t HCI_EVNT_NETAPP_PING_STOP = 0x2004;
  struct hci_evnt_netapp_ping_stop_t {
    int8_t status;
  };

  /**
   * HCI Event NetApp getipconfig.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_NETAPP_GETIPCONFIG_.280x2005.29
   */
  static const uint16_t HCI_EVNT_NETAPP_GETIPCONFIG = 0x2005;
  struct hci_evnt_netapp_getipconfig_t {
    int8_t status;
    uint8_t ip[4];
    uint8_t subnet[4];
    uint8_t gateway[4];
    uint8_t dhcp[4];
    uint8_t dns[4];
    uint8_t mac[6];
    uint8_t ssid[32];
  };

  /**
   * HCI Event WLAN unsol free buff.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_DATA_UNSOL_FREE_BUFF_.280x4100.29
   */
  static const uint16_t HCI_EVNT_DATA_UNSOL_FREE_BUFF = 0x4100;
  struct hci_evnt_data_unsol_free_buff_t {
    int8_t status;
    uint16_t count;
    struct {
      uint16_t magic;
      uint16_t buffers_freed;
    } flow_control_event;
  };

  /**
   * HCI Event WLAN unsol connect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_UNSOL_CONNECT_.280x8001.29
   */
  static const uint16_t HCI_EVNT_WLAN_UNSOL_CONNECT = 0x8001;
  struct hci_evnt_wlan_unsol_connect_t {
    int8_t status;
  };

  /**
   * HCI Event WLAN unsol connect.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_UNSOL_DISCONNECT_.280x8002.29
   */
  static const uint16_t HCI_EVNT_WLAN_UNSOL_DISCONNECT = 0x8002;
  struct hci_evnt_wlan_unsol_disconnect_t {
    int8_t status;
  };

  /**
   * HCI Event WLAN unsol dhcp.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_UNSOL_DHCP_.280x8010.29
   */
  static const uint16_t HCI_EVNT_WLAN_UNSOL_DHCP = 0x8010;
  struct hci_evnt_wlan_unsol_dhcp_t {
    int8_t status;
    uint8_t ip[4];
    uint8_t subnet[4];
    uint8_t gateway[4];
    uint8_t dhcp[4];
    uint8_t dns[4];
  };

  /**
   * HCI Event WLAN unsol ping report.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_UNSOL_PING_REPORT_.280x8040.29
   */
  static const uint16_t HCI_EVNT_WLAN_UNSOL_PING_REPORT = 0x8040;
  struct hci_evnt_wlan_unsol_ping_report_t {
    int8_t status;
    uint32_t echo_requests_sent;
    uint32_t echo_replies_recieved;
    uint32_t max_rtt;
    uint32_t min_rtt;
    uint32_t avg_rtt;
  };

  /**
   * HCI Event WLAN unsol smart config done.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_UNSOL_SMART_CONFIG_DONE_.280x8080.29
   */
  static const uint16_t HCI_EVNT_WLAN_UNSOL_SMART_CONFIG_DONE = 0x8080;
  struct hci_evnt_wlan_unsol_smart_config_done_t {
    int8_t status;
  };

  /**
   * HCI Event WLAN unsol keepalive.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_UNSOL_KEEPALIVE_.280x8200.29
   */
  static const uint16_t HCI_EVNT_WLAN_UNSOL_KEEPALIVE = 0x8200;
  struct hci_evnt_wlan_unsol_keepalive_t {
    int8_t status;
  };

  /**
   * HCI Event WLAN unsol keepalive.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_WLAN_UNSOL_TCP_CLOSE_WAIT_.280x8800.29
   */
  static const uint16_t HCI_EVNT_WLAN_UNSOL_TCP_CLOSE_WAIT = 0x8800;
  struct hci_evnt_wlan_unsol_tcp_close_wait_t {
    int8_t status;
    uint32_t handle;
  };

  /**
   * HCI Event simple link start.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_SIMPLE_LINK_START_.280x4000.29
   */
  static const uint16_t HCI_EVNT_SIMPLE_LINK_START = 0x4000;
  struct hci_evnt_simple_link_start_t {
    int8_t status;
  };

  /**
   * HCI Event read buffer size.
   * http://processors.wiki.ti.com/index.php/CC3000_HCI_EVNT_messages#HCI_EVNT_READ_BUFFER_SIZE_.280x400B.29   *
   */
  static const uint16_t HCI_EVNT_READ_BUFFER_SIZE = 0x400B;
  struct hci_evnt_read_buffer_size_t {
    int8_t status;
    uint8_t count;
    uint16_t bytes;
  };
};

#endif
#endif
