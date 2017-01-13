/**
 * @file W5500.hh
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

#ifndef COSA_W5500_HH
#define COSA_W5500_HH

#include "Cosa/Types.h"

#if !defined(BOARD_ATTINY)
#include "Cosa/SPI.hh"
#include "Cosa/Socket.hh"

/**
 * Cosa WIZnet W5500 device driver class. Provides an implementation
 * of the Cosa Socket and Cosa IOStream::Device classes. A socket may
 * be bound directly to a Cosa IOStream. The device internal
 * transmitter buffer is used. The buffer is sent on flush (TCP/UDP)
 * or when full (TCP). Integrated with Cosa INET/DHCP so that the
 * ethernet controller may obtain a network address and information
 * from a DHCP server.
 *
 * @section Circuit
 * @code
 *                           W5500
 *                       +------------+
 * (D10)--------------29-|CSN         |
 * (D11)--------------28-|MOSI        |
 * (D12)--------------27-|MISO        |
 * (D13)--------------30-|SCK         |
 * (D2)-----[ ]-------56-|IRQ         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. W5500 Datasheet Version 1.0.6, December 30, 2014,
 * http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:w5500:w5500_ds_v106e_141230.pdf
 * 2. W5500 Application Note 1.1  April 09, 2014,
 * http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:w5500:w5500_ap_ipraw_v110e.pdf
 */
class W5500 : private SPI::Driver {
public:
  /**
   * Construct W5500 device driver with given hardware address, and
   * chip select.
   * @param[in] mac hardware address (in program memory, default NULL).
   * @param[in] csn chip selection pin (Default D10).
   */
  W5500(const uint8_t* mac = NULL, Board::DigitalPin csn = Board::D10);

  /**
   * Get the current network address and subnet mask.
   * @param[in] ip network address.
   * @param[in] subnet mask.
   */
  void addr(uint8_t ip[4], uint8_t subnet[4]);

  /**
   * Get DNS network address if W5500 device driver was initiated with
   * hostname and obtained network address from DHCP.
   * @param[in,out] ip network address.
   */
  void dns_addr(uint8_t ip[4]) { memcpy(ip, m_dns, sizeof(m_dns)); }

  /**
   * Initiate W5500 device driver with given hostname. Network address,
   * subnet mask and gateway should be obtained from DHCP. Returns true
   * if successful otherwise false.
   * @param[in] hostname string in program memory.
   * @param[in] timeout retry timeout period (Default 500 ms).
   * @return bool.
   */
  bool begin_P(const char* hostname, uint16_t timeout = 500);
  bool begin_P(str_P hostname, uint16_t timeout = 500)
  {
    return (begin_P((const char*) hostname, timeout));
  }

  /**
   * Initiate W5500 device driver with given network address and subnet
   * mask. Returns true if successful otherwise false.
   * @param[in] ip network address (Default NULL, 0.0.0.0).
   * @param[in] subnet mask (Default NULL, 0.0.0.0).
   * @param[in] timeout retry timeout period (Default 500 ms).
   * @return bool.
   */
  bool begin(uint8_t ip[4] = NULL, uint8_t subnet[4] = NULL,
	     uint16_t timeout = 500);

  /**
   * Bind to the given network address and subnet mask. Returns zero
   * if successful otherwise negative error code.
   * @param[in] ip network address.
   * @param[in] subnet mask.
   * @param[in] gateway network address (Default NULL).
   * @return zero if successful otherwise negative error code.
   */
  int bind(uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4] = NULL);

  /**
   * Allocate socket with the given protocol, port and flags. Returns
   * pointer to socket. The socket is deallocated with Socket::close().
   * @param[in] proto socket protocol.
   * @param[in] port number (Default 0).
   * @param[in] flag (Default 0).
   * @return socket pointer or NULL.
   */
  Socket* socket(Socket::Protocol proto, uint16_t port = 0, uint8_t flag = 0);

  /**
   * Terminate W5500 device driver. Closes all active sockets. Return
   * true if successful otherwise false.
   */
  bool end();

protected:
  /**
   * Common Registers (chap. 3.1, pp. 30), big-endian 16-bit values.
   */
  struct CommonRegister {
    uint8_t MR;			//!< Mode Register.
    uint8_t GAR[4];		//!< Gateway Address Register.
    uint8_t SUBR[4];		//!< Subnet mask Address Register.
    uint8_t SHAR[6];		//!< Source Hardware Address Register.
    uint8_t SIPR[4];		//!< Source IP Address Register.
    uint16_t INTLEVEL;		//!< Interrupt Low level Timer.
    uint8_t IR;			//!< Interrupt Register.
    uint8_t IMR;		//!< Interrupt Mask Register.
    uint8_t SIR;		//!< Socket Interrupt Register.
    uint8_t SIMR;		//!< Socket Interrupt Mask Register.
    uint16_t RTR;		//!< Retry Time Register.
    uint8_t RCR;		//!< Retry Count Register.
    uint8_t PTIMER;		//!< PPP LCP Request Timer Register.
    uint8_t PMAGIC;		//!< PPP LCP Magic number.
    uint8_t PHAR[6];		//!< PPP Destination MAC Address
    uint16_t PSID;		//!< PPP Session Identification.
    uint16_t PMRU;		//!< PPP Maximum Segment Size.
    uint8_t UIPR[4];		//!< Unreachable IP Address Register
    uint16_t UPORTR;		//!< Unreachable Port Register
    uint8_t PHYCFGR;		//!< PHY COnfiguration
    uint8_t reserved[10];	//!< Reserved
    uint8_t VERSIONR;		//!< Chip Version
  };

  /**
   * Mode Register bitfields, pp. 33-34.
   */
  enum {
    MR_RST = 0x80,		//!< S/W Reset.
    MR_WOL = 0x20,		//!< Wake on LAN
    MR_PB = 0x10,		//!< Ping Block Mode.
    MR_PPPoE = 0x08,		//!< PPPoE Mode.
    MR_FARP = 0x02		//!< Force ARP
  } __attribute__((packed));

  /**
   * Interrupt Register bitfields, pp. 36.
   */
  enum {
    IR_CONFLICT = 0x80,		//!< IP Conflict.
    IR_UNREACH = 0x40,		//!< Destination Unreachable
    IR_PPPoE = 0x20,		//!< PPPoE Connection Close.
    IR_MP = 0x10		//!< Magic Packet
  } __attribute__((packed));

  /*
   * Interrupt Mask Register bitfields, pp. 37.
   */
  enum {
    IMR_CONFLICT = 0x80,	//<! IP Conflict.
    IMR_UNREACH = 0x40,		//!< Destination Unreachable
    IMR_PPPoE = 0x20,		//!< PPPoE Connection Close.
    IMR_MP = 0x10		//!< Magic Packet
  } __attribute__((packed));
 
  /**
   * Socket Interrupt Register bitfields, pp. 38.
   */
  enum {
    SIR_S7_INT = 0x80,		//!< Occurrence of Socket 7 Interrupt.
    SIR_S6_INT = 0x40,		//!< Occurrence of Socket 6 Interrupt.
    SIR_S5_INT = 0x20,		//!< Occurrence of Socket 5 Interrupt.
    SIR_S4_INT = 0x10,		//!< Occurrence of Socket 4 Interrupt.
    SIR_S3_INT = 0x08,		//!< Occurrence of Socket 3 Interrupt.
    SIR_S2_INT = 0x04,		//!< Occurrence of Socket 2 Interrupt.
    SIR_S1_INT = 0x02,		//!< Occurrence of Socket 1 Interrupt.
    SIR_S0_INT = 0x01		//!< Occurrence of Socket 0 Interrupt.
  } __attribute__((packed));

  /**
   * Socket Interrupt Mask Register bitfields, pp. 38.
   */
  enum {
    SIMR_S7_INT = 0x80,		//!< Mask occurrence of Socket 7 Interrupt.
    SIMR_S6_INT = 0x40,		//!< Mask occurrence of Socket 6 Interrupt.
    SIMR_S5_INT = 0x20,		//!< Mask occurrence of Socket 5 Interrupt.
    SIMR_S4_INT = 0x10,		//!< Mask occurrence of Socket 4 Interrupt.
    SIMR_S3_INT = 0x08,		//!< Mask occurrence of Socket 3 Interrupt.
    SIMR_S2_INT = 0x04,		//!< Mask occurrence of Socket 2 Interrupt.
    SIMR_S1_INT = 0x02,		//!< Mask occurrence of Socket 1 Interrupt.
    SIMR_S0_INT = 0x01		//!< Mask occurrence of Socket 0 Interrupt.
  } __attribute__((packed));

  /**
   * RX/TX Socket Memory Size bitfield pp. 54-55.
   */
  enum {
    MEM_SIZE_00K = 0x00,    	//!< 0KB
    MEM_SIZE_01K = 0x01,    	//!< 1KB
    MEM_SIZE_02K = 0x02,    	//!< 2KB
    MEM_SIZE_04K = 0x04,    	//!< 4KB
    MEM_SIZE_08K = 0x08,    	//!< 8KB
    MEM_SIZE_16K = 0x10,    	//!< 16KB
  } __attribute__((packed));

  /**
   * Socket Registers (chap. 3.2 pp. 31).
   */
  struct SocketRegister {
    uint8_t MR;			//!< Mode Register.
    uint8_t CR;			//!< Command Register.
    uint8_t IR;			//!< Interrupt Register.
    uint8_t SR;			//!< Status Register.
    uint16_t PORT;		//!< Source Port Register.
    uint8_t DHAR[6];		//!< Destination Hardware Address Register.
    uint8_t DIPR[4];		//!< Destination IP Address Register.
    uint16_t DPORT;		//!< Destination Port Register.
    uint16_t MSSR;		//!< Maximum Segment Size Register.
    uint8_t PROTO;		//!< Protocol in IP Raw mode. See W5500 Application Note doc
    uint8_t TOS;		//!< IP TOS.
    uint8_t TTL;		//!< IP TTL.
    uint8_t reserved1[7];	//!< Reserved.
    uint8_t RXMEM_SIZE;		//!< RX Memory Size Register.
    uint8_t TXMEM_SIZE;		//!< TX Memory Size Register.
    uint16_t TX_FSR;		//!< TX Free Size Register.
    uint16_t TX_RD;		//!< TX Read Pointer Register.
    uint16_t TX_WR;		//!< TX Write Pointer Register.
    uint16_t RX_RSR;		//!< RX Received Size Register.
    uint16_t RX_RD;		//!< RX Read Pointer Register.
    uint16_t RX_WR;		//!< RX Write Pointer Register.
    uint8_t IMR;		//!< Interrupt Mask Register.
    uint16_t FRAG;		//!< Fragment Register.
    uint8_t KPALVTR;		//!< Keep alive timer.
  };

  /**
   * Socket Mode Register bitfields, pp 45-46.
   */
  enum {
    MR_FLAG_MASK = 0xe0,	//!< Flag mask.
    MR_MULTIMF = 0x80,		//!< Multicast(UDP) MAC Filter(MACRAW).
    MR_BCASTB = 0x40,		//!< Broadcast Block(UDP & MACRAW)
    MR_NDMCMMB = 0x20,		//!< No Delayed ACK(TCP) Multicast IGMP version(UDP) Multicast Block(MACRAW).
    MR_UCASTB = 0x10,		//!< Unicast Block(UDP) IPv6 Block(MACRAW)
    MR_PROTO_MASK = 0x0f,	//!< Protocol mask.
    MR_PROTO_CLOSED = 0x00,	//!< Closed.
    MR_PROTO_TCP = 0x01,	//!< TCP.
    MR_PROTO_UDP = 0x02,	//!< UDP.
    MR_PROTO_IPRAW = 0x03,	//!< RAW IP. See W5500 Application Note doc
    MR_PROTO_MACRAW = 0x04,	//!< RAW MAC.
  } __attribute__((packed));

  /**
   * Socket Command Register values, pp. 47-49
   */
  enum {
    CR_OPEN = 0x01,		//!< Initiate socket according to MR.
    CR_LISTEN = 0x02,		//!< TCP: Initiate server mode.
    CR_CONNECT = 0x04,		//!< TCP: Initiate client mode.
    CR_DISCON = 0x08,		//!< TCP: Disconnect server/client.
    CR_CLOSE = 0x10,		//!< Close socket.
    CR_SEND = 0x20,		//!< Transmit data according to TX_WR.
    CR_SEND_MAC = 0x21,		//!< UDP: Transmit data.
    CR_SEND_KEEP = 0x22,	//!< TCP: Check connection status.
    CR_RECV = 0x40		//!< Receiving packet to RX_RD.
  } __attribute__((packed));

  /**
   * Socket Interrupt Register bitfields, pp. 49.
   */
  enum {
    IR_SEND_OK = 0x10,		//!< Send operation is completed.
    IR_TIMEOUT = 0x08,		//!< Timeout occured.
    IR_RECV = 0x04,		//!< Received data.
    IR_DISCON = 0x02,		//!< Connection termination.
    IR_CON = 0x01		//!< Connection established.
  } __attribute__((packed));

  /**
   * Socket Status Register values, pp. 50-51.
   */
  enum {
    SR_CLOSED = 0x00,
    SR_ARP = 0x01,		//!< Undocumented ????
    SR_INIT = 0x13,
    SR_LISTEN = 0x14,
    SR_SYNSENT = 0x15,
    SR_SYNRECV = 0x16,
    SR_ESTABLISHED = 0x17,
    SR_FIN_WAIT = 0x18,
    SR_CLOSING = 0x1A,
    SR_TIME_WAIT = 0x1B,
    SR_CLOSE_WAIT = 0x1C,
    SR_LAST_ACK = 0x1D,
    SR_UDP = 0x22,
    SR_IPRAW = 0x32,
    SR_MACRAW = 0x42,
    SR_PPPoE = 0x5F		//!< Undocumented ????
  } __attribute__((packed));

  /**
   * SPI Control Phase bits and masks, pp. 16-17.
   * CP byte made up of (BSB & RWB & OM & (SN << 5))
   * For BSB selecting control register no socket number should ever be given.
   */
  enum {
    SPI_CP_BSB_CR = 0x00,	//!< Control Register
    SPI_CP_BSB_SR = 0x08,	//!< Socket Register
    SPI_CP_BSB_TX = 0x10,	//!< Socket TX Buffer
    SPI_CP_BSB_RX = 0x18,	//!< Socket RX Buffer
    SPI_CP_RWB_RS = 0x00,	//!< Read Access Select
    SPI_CP_RWB_WS = 0x04,	//!< Write Access Select
    SPI_CP_OM_VDM = 0x00,	//!< Operation, variable date mode
    SPI_CP_OM_FD1 = 0x01,	//!< Operation, fixed data 1 byte mode
    SPI_CP_OM_FD2 = 0x02,	//!< Operation, fixed data 2 byte mode
    SPI_CP_OM_FD4 = 0x03,	//!< Operation, fixed data 3 byte mode
  } __attribute__((packed));

  /** Socket Buffer Size; 2 Kbyte TX/RX per socket. */
  static const size_t BUF_MAX = 2048;  // this is initial, max is really 16KB

  /** TX Message Size; internal buffer size for flush threshold. */
  static const size_t MSG_MAX = BUF_MAX / 2;

  /** Maximum number of sockets on device. */
  static const uint8_t SOCK_MAX = 8;

  /** Maximum number of DNS request retries. */
  static const uint8_t DNS_RETRY_MAX = 4;

public:
  /**
   * W5500 Single-Chip Internet-enable 10/100 Ethernet Controller Driver.
   * Implements the Cosa/Socket interface.
   */
  class Driver : public Socket {
    friend class W5500;
  public:
    /** Default constructor. */
    Driver() : Socket() {}

    /**
     * @override{IOStream::Device}
     * Number of bytes available in receiver buffer.
     * @return bytes.
     */
    virtual int available();

    /**
     * @override{IOStream::Device}
     * Number of bytes room in transmitter buffer.
     * @return bytes.
     */
    virtual int room();

    /** Overloaded virtual member function write. */
    using IOStream::Device::write;

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
     * Initiate socket to the given protocol and possible
     * port. Returns zero if successful otherwise negative error code;
     * -2 already open, -1 failed to open socket.
     * @param[in] proto protocol.
     * @param[in] port source port.
     * @param[in] flag socket options.
     * @return zero if successful otherwise negative error code.
     */
    virtual int open(Protocol proto, uint16_t port, uint8_t flag);

    /**
     * @override{Socket}
     * Close the socket. Returns zero if successful otherwise negative
     * error code; -2 already closed.
     * @param[in] proto protocol.
     * @param[in] port source port.
     * @param[in] flag socket options.
     * @return zero if successful otherwise negative error code.
     */
    virtual int close();

    /**
     * @override{Socket}
     * Mark socket for incoming requests; server mode. Returns zero if
     * successful otherwise negative error code; -2 illegal protocol,
     * -1 failed to mark socket for listen (socket is closed).
     * @return zero if successful otherwise negative error code.
     */
    virtual int listen();

    /**
     * @override{Socket}
     * Check for incoming requests from clients. Return zero if the
     * socket has accepted a request and a connection is established,
     * otherwise a negative error code; -3 listening or connection in
     * progress, -2 illegal protocol, -1 illegal state (socket is
     * closed).
     * @return zero if successful otherwise negative error code.
     */
    virtual int accept();

    /**
     * @override{Socket}
     * Connect the socket to the given address and port; client mode.
     * Returns a zero if successful otherwise a negative error code;
     * -2 illegal protocol, -1 address/port not valid.
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
     * Returns positive integer if a connection is established, zero
     * is not yet established, otherwise a negative error code.
     * @return positive integer connected, zero if not otherwise
     * negative error code.
     */
    virtual int is_connected();

    /**
     * @override{Socket}
     * Disconnect socket from server. Returns zero if successful
     * otherwise a negative error code; -2 illegal protocol.
     * @return zero if successful otherwise negative error code.
     */
    virtual int disconnect();

    /**
     * @override{Socket}
     * Start the construction of a datagram to the given address and
     * port. Checks that the socket is connection-less (UDP). Returns
     * zero or negative error code.
     * @param[in] addr destination address.
     * @param[in] port destination port.
     * @return zero if successful otherwise negative error code.
     */
    virtual int datagram(uint8_t addr[4], uint16_t port);

    /**
     * @override{Socket}
     * Receive data from connection-oriented socket. The data is stored
     * in given buffer with given maximum number of bytes. Return number of
     * bytes or negative error code; -3 socket not established,
     * -2 illegal protocol.
     * @param[in] buf buffer pointer.
     * @param[in] len number of bytes in buffer.
     * @return number of bytes sent if successful otherwise negative
     * error code.
     */
    virtual int recv(void* buf, size_t len);

    /**
     * @override{Socket}
     * Receive datagram on connectionless socket into given buffer
     * with given maximum size. Returns zero(0) if successful with
     * information in Datagram otherwise negative error code;
     * -2 illegal protocol.
     * @param[in] buf buffer pointer.
     * @param[in] len number of bytes in buffer.
     * @param[in] src source address.
     * @param[in] port source port.
     * @return number of bytes received if successful otherwise
     * negative error code.
     */
    virtual int recv(void* buf, size_t len,
		     uint8_t src[4], uint16_t& port);

  protected:
    /** Pointer to socket registers; symbolic address calculation. */
    SocketRegister* m_sreg;

    /** Pointer to device context. */
    W5500* m_dev;

    /** Socket number. */
    uint8_t m_snum;

    /** Offset in socket transmitter buffer. */
    uint16_t m_tx_offset;

    /** Length of message in socket transmitter buffer. */
    uint16_t m_tx_len;

    /**
     * Read data from the socket receiver buffer to the given buffer
     * with the given maximum size.
     * @param[in] buf pointer to buffer for data.
     * @param[in] len maximum number of bytes in buffer.
     * @return number of bytes read if successful otherwise negative
     * error code.
     */
    int dev_read(void* buf, size_t len);

    /**
     * Write data to the socket transmitter buffer from the given buffer
     * with the given number of bytes.
     * @param[in] buf pointer to buffer with data.
     * @param[in] len number of bytes in buffer.
     * @param[in] progmem program memory pointer flag.
     * @return number of bytes written if successful otherwise negative
     * error code.
     */
    int dev_write(const void* buf, size_t len, bool progmem);

    /**
     * Flush any waiting data in the socket receiver buffer.
     */
    void dev_flush();

    /**
     * Wait for given maximum message size in internal transmit buffer.
     * Setup transmitter offset and initiate length for new message
     * construction.
     */
    void dev_setup();

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
     * of bytes or negative error code; -4 socket closed by peer, -3
     * connection not estabilished, -2 illegal protocol.
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
     * destination address (dest:port). Return number of bytes sent or
     * negative error code; -2 illegal protocol, -1 illegal
     * destination address or port.
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
  };

  /** Default hardware network address. */
  static const uint8_t MAC[6] PROGMEM;

protected:
  /** Sockets on device. */
  Driver m_sock[SOCK_MAX];

  /** Pointer to common registers; symbolic field calculation. */
  CommonRegister* m_creg;

  /** Next local port number; DYNAMIC_PORT(49152)-UINT16_MAX(65535). */
  uint16_t m_local;

  /** Hardware address (in program memory). */
  const uint8_t* m_mac;

  /** DNS server network address (provided by DHCP). */
  uint8_t m_dns[4];

  /**
   * Write byte to given address.
   * @param[in] addr address on device.
   * @param[in] ctl byte specifiying what block of memory to access, BSB of Control Phase byte
   * @param[in] data to write.
   */
  void write(uint16_t addr, uint8_t ctl, uint8_t data)
  {
    write(addr, ctl, &data, 1);
  }

  /**
   * Write data from given buffer with given number of bytes to address.
   * @param[in] addr address on device.
   * @param[in] ctl byte specifiying what block of memory to access, BSB of Control Phase byte
   * @param[in] buf pointer to buffer.
   * @param[in] len number of bytes to write.
   * @param[in] progmem program memory pointer flag.
   */
  void write(uint16_t addr, uint8_t ctl, const void* buf, size_t len, bool progmem = false);

  /**
   * Write data from given program memory buffer with given number of
   * bytes to address.
   * @param[in] addr address on device.
   * @param[in] ctl byte specifiying what block of memory to access, BSB of Control Phase byte
   * @param[in] buf pointer to buffer in program memory.
   * @param[in] len number of bytes to write.
   */
  void write_P(uint16_t addr, uint8_t ctl, const void* buf, size_t len)
  {
    write(addr, ctl, buf, len, true);
  }

  /**
   * Read byte from given address.
   * @param[in] addr address on device.
   * @param[in] ctl byte specifiying what block of memory to access, BSB of Control Phase byte
   */
  uint8_t read(uint16_t addr, uint8_t ctl);

  /**
   * Read data from given address on device to given buffer with given
   * number of bytes.
   * @param[in] addr address on device.
   * @param[in] ctl byte specifiying what block of memory to access, BSB of Control Phase byte
   * @param[in] buf pointer to buffer.
   * @param[in] len number of bytes to read.
   */
  void read(uint16_t addr, uint8_t ctl, void* buf, size_t len);

  /**
   * Issue given command to register with given address and await
   * completion.
   * @param[in] addr address on device.
   * @param[in] ctl byte specifiying what block of memory to access, BSB of Control Phase byte
   * @param[in] cmd command to issue.
   */
  void issue(uint16_t addr, uint8_t ctl, uint8_t cmd);
};

#endif
#endif
