/**
 * @file Cosa/Socket/Driver/NRF24L01P.hh
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

#ifndef __COSA_SOCKET_DRIVER_NRF24L01P_HH__
#define __COSA_SOCKET_DRIVER_NRF24L01P_HH__

#include "Cosa/SPI.hh"
#include "Cosa/Pins.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Event.hh"
#include "Cosa/Socket.hh"

/**
 * Socket device driver for the nRF24L01+ Single Chip 2.4GHz
 * Transceiver. Allow nRF24L01+ be used as a device in the socket
 * interface. Supports datagrams and connection-oriented communication
 * using the nRF24L01+ pipes. For further details on the Socket
 * interface see Socket.hh and nRF24L01+ Product Specification (Rev. 1.0) 
 * http://www.nordicsemi.com/kor/nordic/download_resource/8765/2/17776224
 */
class NRF24L01P : private SPI::Driver, public Socket::Device {
public:
  /**
   * Configuration max values
   */
  enum {
    PAYLOAD_MAX = 32		// Max size of payload
  } __attribute__((packed));

private:
  /**
   * NRF transceiver states (See chap. 6.1.1, fig. 4, pp. 22)
   */
  enum State {
    POWER_DOWN_STATE = 0,
    STANDBY_STATE,
    RX_STATE,
    TX_STATE
  } __attribute__((packed));

  /**
   * SPI Commands (See chap. 8.3.1, tab. 20, pp. 51)
   */
  enum Command {
    R_REGISTER = 0x00,		// Read command and status register
    W_REGISTER = 0x20,		// Write command and status register
    REG_MASK = 0x1f,		// Mask register address (5b)
    R_RX_PAYLOAD = 0x61,	// Read RX payload
    W_TX_PAYLOAD = 0xa0,	// Write TX payload
    FLUSH_TX = 0xe1,		// Flush TX FIFO
    FLUSH_RX = 0xe2,		// Flush RX FIFO
    REUSE_TX_PL = 0xe3,		// Reuse last transmitted payload
    R_RX_PL_WID = 0x60,		// Read RX payload width
    W_ACK_PAYLOAD = 0xa8,	// Write TX payload with ACK (3 bit addr)
    PIPE_MASK = 0x07,		// Mask pipe address
    W_ACK_PAYLOAD_NOACK = 0xb0,	// Disable AUTOACK on this specific packet
    NOP = 0xff			// No operation, return status
  } __attribute__((packed));

  /**
   * NRF transceiver registers map (See chap. 9, tab. 28, pp. 57)
   */
  enum Register {
    CONFIG = 0x00,		// Configuration register
    EN_AA = 0x01,		// Enable auto acknowledgement
    EN_RXADDR = 0x02,		// Enable rx addresses
    SETUP_AW = 0x03,		// Setup of address width
    SETUP_RETR = 0x04,		// Setup of auto retransmission
    RF_CH = 0x05,		// RF channel
    RF_SETUP = 0x06,		// RF setup register
    STATUS = 0x07,		// Status register
    OBSERVE_TX = 0x08,		// Transmit observe register
    RPD = 0x09,			// Received power detector
    RX_ADDR_P0 = 0x0a,		// Receive address data pipe 0
    RX_ADDR_P1 = 0x0b,		// - data pipe 1
    RX_ADDR_P2 = 0x0c,		// - data pipe 2
    RX_ADDR_P3 = 0x0d,		// - data pipe 3
    RX_ADDR_P4 = 0x0e,		// - data pipe 4
    RX_ADDR_P5 = 0x0f,		// - data pipe 5
    TX_ADDR = 0x10,		// Transmit address
    RX_PW_P0 = 0x11,		// Number of bytes in RX payload in data pipe 0
    RX_PW_P1 = 0x12,		// - data pipe 1
    RX_PW_P2 = 0x13,		// - data pipe 2
    RX_PW_P3 = 0x14,		// - data pipe 3
    RX_PW_P4 = 0x15,		// - data pipe 4
    RX_PW_P5 = 0x16,		// - data pipe 5
    FIFO_STATUS = 0x17,		// FIFO status register
    DYNPD = 0x1c,		// Enable dynamic payload length
    FEATURE = 0x1d		// Feature register
  } __attribute__((packed));

  /**
   * Register CONFIG bitfields, configuration
   */
  enum {
    MASK_RX_DR = 6,		// Mask interrupt caused by RX_DR
    MASK_TX_DS = 5,		// Mask interrupt caused by TX_DS
    MASK_MAX_RT = 4,		// Mask interrupt caused byt MAX_RT
    EN_CRC = 3,			// Enable CRC
    CRCO = 2,			// CRC encoding scheme (2/1 bytes CRC)
    PWR_UP = 1,			// Power up/down
    PRIM_RX = 0,		// RX/TX control (PRX/PTX)
  } __attribute__((packed));

  enum {
    POWER_DOWN = 0,		// PWR_UP bit settings
    POWER_UP = _BV(PWR_UP)
  } __attribute__((packed));

  /**
   * Register EN_AA bitfields, auto acknowledgement
   */
  enum {
    ENAA_P5 = 5,		// Enable auto acknowledgement data pipe 5
    ENAA_P4 = 4,		// - data pipe 4
    ENAA_P3 = 3,		// - data pipe 3
    ENAA_P2 = 2,		// - data pipe 2
    ENAA_P1 = 1,		// - data pipe 1
    ENAA_P0 = 0,		// - data pipe 0
    ENAA_PA = 0x3f		// Enable all auto ack on all data pipes
  } __attribute__((packed));
  
  /**
   * Register EN_RXADDR bitfields, enable receive pipe
   */
  enum {
    ERX_P5 = 5,			// Enable data pipe 5
    ERX_P4 = 4,			// - data pipe 4
    ERX_P3 = 3,			// - data pipe 3
    ERX_P2 = 2,			// - data pipe 2
    ERX_P1 = 1,			// - data pipe 1
    ERX_P0 = 0,			// - data pipe 0
    ERX_PA = 0x3f		// Enable all data pipes
  } __attribute__((packed));

  /**
   * Register SETUP_AW bitfields, setup address width (3..5)
   */
  enum {
    AW = 0,			// RX/TX address field width (bits 2)
    AW_3BYTES = 1,		// 3 bytes
    AW_4BYTES = 2,		// 4 bytes
    AW_5BYTES = 3		// 5 bytes
  } __attribute__((packed));

  /**
   * Register SETUP_RETR bitfields, configure retransmission
   */
  enum {
    ARD = 4,			// Auto retransmit delay (bits 4)
    				// - delay * 250 us (250..4000 us)
    ARC = 0			// Auto retransmit count (bits 4)
    				// - retransmit count (0..15)
  } __attribute__((packed));

  /**
   * Register RF_SETUP bitfields, radio configuration
   */
  enum {
    CONT_WAVE = 7,	    	// Continuous carrier transmit
    RF_DR_LOW = 5,		// Set RF data rate to 250 kbps
    PLL_LOCK = 4,		// Force PLL lock signal
    RF_DR_HIGH = 3,		// Air data bitrate (2 Mbps)
    RF_PWR = 1			// Set RF output power in TX mode (bits 2)
  } __attribute__((packed));
  
  /**
   * Transmission rates RF_DR_LOW/RF_DR_HIGH values, radio bit-rate
   */
  enum {
    RF_DR_1MBPS = 0,			// 1 Mbps
    RF_DR_2MBPS = _BV(RF_DR_HIGH), 	// 2 Mbps
    RF_DR_250KBPS = _BV(RF_DR_LOW)	// 250 Kbps
  } __attribute__((packed));

  /**
   * Output power RF_PWR values, radio power setting
   */
  enum {
    RF_PWR_18DBM = 0,		// -18dBm
    RF_PWR_12DBM = 2,		// -12dBm
    RF_PWR_6DBM = 4,		// -6dBm
    RF_PWR_0DBM = 6		//  0dBm
  } __attribute__((packed));

  /**
   * Register STATUS bitfields
   */
  enum {
    RX_DR = 6,			// Data ready RX FIFO interrupt
    TX_DS = 5,			// Data send TX FIFO interrupt
    MAX_RT = 4,			// Maximum number of TX retransmits interrupt
    RX_P_NO = 1,		// Data pipe number for available payload (3b)
    RX_P_NO_MASK = 0x0e,	// Mask pipe number
    RX_P_NO_NONE = 0x07,	// No pipe
    TX_FIFO_FULL = 0		// TX FIFO full flag
  } __attribute__((packed));

  /**
   * Register STATUS data type
   */
  union status_t {
    uint8_t as_byte;
    struct {
      uint8_t tx_full:1;	// TX FIFO full
      uint8_t rx_p_no:3;	// Data pipe number for available payload
      uint8_t max_rt:1;		// Maximum number of TX retransmit interrupt
      uint8_t tx_ds:1;		// Data send TX FIFO interrupt
      uint8_t rx_dr:1;		// Data ready RX FIFO interrupt
    };

    /**
     * Construct status from register reading.
     * @param[in] value register reading.
     */
    status_t(uint8_t value) 
    { 
      as_byte = value;
    }
  };

  /**
   * Register OBSERVE_TX bitfields, performance statistics
   */
  enum  {
    PLOS_CNT = 4,		// Count lost packets (bits 4)
    ARC_CNT = 0		        // Count retransmitted packets (bits 4)
  } __attribute__((packed));

  /**
   * Register OBSERVE_TX data type, performance statistics
   */
  union observe_tx_t {
    uint8_t as_byte;
    struct {
      uint8_t arc_cnt:4;	// Count retransmitted packets 
      uint8_t plos_cnt:4;	// Count lost packets
    };

    /**
     * Construct transmitter performance statistics from register
     * reading.
     * @param[in] value register reading.
     */
    observe_tx_t(uint8_t value)
    {
      as_byte = value;
    }
  };

  /**
   * Register FIFO_STATUS bitfields, transmission queue status
   */
  enum {
    TX_REUSE = 6,		// Reuse last transmitted data packat
    TX_FULL = 5,		// TX FIFO full flag
    TX_EMPTY = 4,		// TX FIFO empty flag
    RX_FULL = 1,		// RX FIFO full flag
    RX_EMPTY = 0,		// RX FIFO empty flag
  } __attribute__((packed));
  
  /**
   * Register FIFO_STATUS data type, transmission queue status
   */
  union fifo_status_t {
    uint8_t as_byte;
    struct {
      uint8_t rx_empty:1;	// RX FIFO empty flag
      uint8_t rx_full:1;	// RX FIFO full flag
      uint8_t reserved:2;
      uint8_t tx_empty:1;	// TX FIFO empty flag
      uint8_t tx_full:1;	// TX FIFO full flag
      uint8_t tx_reuse:1;	// Reuse last transmitted data packat
    };

    /**
     * Construct transmitter queue status from register reading.
     * @param[in] value register reading.
     */
    fifo_status_t(uint8_t value)
    {
      as_byte = value;
    }
  };
  
  /**
   * Register DYNPD bitfields
   */
  enum {
    DPL_P5 = 5,			// Enable dynamic payload length data pipe 5
    DPL_P4 = 4,			// - data pipe 4
    DPL_P3 = 3,			// - data pipe 3
    DPL_P2 = 2,			// - data pipe 2
    DPL_P1 = 1,			// - data pipe 1
    DPL_P0 = 0,			// - data pipe 0
    DPL_PA = 0x3f		// Enable dynamic payload length on all pipes
  } __attribute__((packed));

  /**
   * Register FEATURE bitfields
   */
  enum {
    EN_DPL = 2,			// Enable dynamic payload length
    EN_ACK_PAY = 1,		// Enable payload with ACK
    EN_DYN_ACK = 0		// Enable the W_TX_PAYLOAD_NOACK command
  } __attribute__((packed));

  /**
   * Configuration max values
   */
  enum {
    AW_MAX = 5,                 // Max address width in bytes
    PIPE_MAX = 6,		// Max number of pipes
  } __attribute__((packed));

  /** Datagrams are sent/received on pipe(0), others are connections */
  static const uint8_t DATAGRAM_PIPE = 0;
  
  /**
   * Datagram header with destination port and source address/node.
   * Destination address is in hardware register (RX_ADDR_P0/P1).
   */
  struct header_t {
    struct {			// Destination node address is pipe address
      uint16_t port;		// Destination port
    } dest;
    Socket::addr_t src;		// Source node address and port
  };

  /** Maximum size of payload for datagram */
  static const uint8_t DATAGRAM_MAX = PAYLOAD_MAX - sizeof(header_t);

  /**
   * Connection request message operations
   */
  enum {
    CONNECT_REQUEST = 1,
    CONNECT_RESPONSE,
    DISCONNECT_REQUEST,
    DISCONNECT_RESPONSE
  } __attribute__((packed));

  /**
   * Connection request/response message
   */
  struct request_t {
    uint8_t op;			// Request/response operation code
    uint16_t param;		// Parameter (optional)
  };
  
  /**
   * Interrupt handler
   */
  class IRQPin : public ExternalInterrupt {
    friend class NRF24L01P;
  private:
    NRF24L01P* m_nrf;
  public:
    IRQPin(Board::ExternalInterruptPin pin, Mode mode, NRF24L01P* nrf) : 
      ExternalInterrupt(pin, mode),
      m_nrf(nrf)
    {}
    virtual void on_interrupt(uint16_t arg = 0);
  };
  
  /** Slave select pin */
  OutputPin m_csn;

  /**Chip enable activity RX/TX select pin */
  OutputPin m_ce;

  /** Chip interrupt pin */
  IRQPin m_irq;
  
  /** Transceiver state */
  State m_state;

  /** Transmission channel and status */
  uint8_t m_channel;
  uint8_t m_status;

  /** Number of messages transmitted and received */
  uint16_t m_nr_tx;
  uint16_t m_nr_rx;

  /** Mapping from pipe to sockets */
  static const uint8_t CLIENT_MAX = PIPE_MAX;
  Client* m_client[CLIENT_MAX];
  uint8_t m_clients;

  /**
   * Attach given client socket to device. Return socket binding 
   * (allocated pipe) or negative error code.
   * @param[in] c client socket to attach.
   * @return binding or negative error code.
   */
  int8_t attach(Client* c);

  /**
   * Detach given client socket from device. 
   * @param[in] c client socket to detach.
   */
  void detach(Client* c);

  /**
   * Map source address and port to the destination device address.
   * Prepare for loading ADDR_RX/P0/P1 register (little endian);
   * @param[out] dest device address (size must be AW_MAX).
   * @param[in] src network address.
   * @param[in] port address.
   */
  void set_address(uint8_t* dest, uint32_t src, uint8_t port)
  {
    *dest++ = port;
    *((uint32_t*) dest) = src;
  }
  
  /**
   * Read command and status registers. Issue R_REGISTER command.
   * @param[in] reg register address.
   * @return register value.
   */
  uint8_t read(Register reg)
  {
    uint8_t res;
    asserted(m_csn) {
      res = spi.read(R_REGISTER | (REG_MASK & reg));
    }
    return (res);
  }

  /**
   * Write command and status registers. Issue W_REGISTER command.
   * @param[in] reg register address.
   * @param[in] data new setting.
   * @return status.
   */
  uint8_t write(Register reg, uint8_t data)
  {
    asserted(m_csn) {
      m_status = spi.write(W_REGISTER | (REG_MASK & reg), data);
    }
    return (m_status);
  }

  /**
   * Write command and status registers. Issue W_REGISTER command and
   * multiple values.
   * @param[in] reg register address.
   * @param[in] buffer data storage.
   * @param[in] count number of bytes to write.
   * @return status.
   */
  uint8_t write(Register reg, const void* buffer, uint8_t count)
  {
    asserted(m_csn) {
      m_status = spi.write(W_REGISTER | (REG_MASK & reg), buffer, count);
    }
    return (m_status);
  }
  
  /**
   * @override
   * Handle receive message from interrupt handler. Reads message from
   * the device and dispatches to the Socket::on_recv() method for
   * bound sockets. Handles listen server connect/disconnect messages.
   * @param[in] type the event type.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);

public:
  /**
   * Construct NRF transceiver with given channel and pin numbers 
   * for SPI slave select, activity enable and interrupt. Default
   * in parenthesis (Standard/Mega Arduino/TinyX4/TinyX5).
   * @param[in] channel number (default 64).
   * @param[in] csn spi slave select pin number (default D10/D53/D2/D3).
   * @param[in] ce chip enable activates pin number (default D9/D48/D3/D4).
   * @param[in] irq interrupt pin number (default EXT0/EXT4/EXT0/EXT0).
   */
#if defined(__ARDUINO_MEGA__)
  NRF24L01P(uint32_t addr,
	    uint8_t channel = 64, 
	    Board::DigitalPin csn = Board::D53, 
	    Board::DigitalPin ce = Board::D48, 
	    Board::ExternalInterruptPin irq = Board::EXT4);
#elif defined(__ARDUINO_TINYX4__)
  NRF24L01P(uint32_t addr,
	    uint8_t channel = 64, 
	    Board::DigitalPin csn = Board::D2, 
	    Board::DigitalPin ce = Board::D3, 
	    Board::ExternalInterruptPin irq = Board::EXT0);
#elif defined(__ARDUINO_TINYX5__)
  NRF24L01P(uint32_t addr,
	    uint8_t channel = 64, 
	    Board::DigitalPin csn = Board::D3, 
	    Board::DigitalPin ce = Board::D4, 
	    Board::ExternalInterruptPin irq = Board::EXT0);
#else // __ARDUINO_STANDARD__ || __ARDUINO_MIGHTY__
  NRF24L01P(uint32_t addr,
	    uint8_t channel = 64, 
	    Board::DigitalPin csn = Board::D10, 
	    Board::DigitalPin ce = Board::D9, 
	    Board::ExternalInterruptPin irq = Board::EXT0);
#endif

  /**
   * Get status of latest operation. Issue NOP command to read status.
   * @return status.
   */
  uint8_t get_status()
  {
    asserted(m_csn) {
      m_status = spi.exchange(NOP);
    }
    return (m_status);
  }

  /**
   * Enable interrupt handler.
   */
  void enable()
  {
    m_irq.enable();
  }

  /**
   * Disable interrupt handler.
   */
  void disable()
  {
    m_irq.disable();
  }

  /**
   * Set power up mode. Will initiate radio with necessary settings
   * after power on reset. 
   */
  void set_powerup_mode();

  /**
   * Set transmitter mode with the given address for transmitter and
   * receiver address for pipe 0 (acknowledgements).
   * @param[in] addr address.
   * @param[in] port address.
   */
  void set_transmitter_mode(uint32_t addr = 0L, uint8_t port = 0);

  /**
   * Set receiver mode and use the given socket device address.
   */
  void set_receiver_mode();

  /**
   * Set standby mode. 
   */
  void set_standby_mode();

  /**
   * Set power down. Turn off radio and go into low power mode. 
   */
  void set_powerdown_mode();

  /**
   * Return true(1) if device is ready, otherwise false(0).
   * @return boolean.
   */
  bool is_ready()
  {
    fifo_status_t fifo = read(FIFO_STATUS);
    return (!fifo.tx_full);
  }

  /**
   * Return true(1) if max retransmit attempts otherwise false(0).
   * @return boolean.
   */
  bool is_max_retransmit()
  {
    status_t status = get_status();
    return (status.max_rt);
  }

  /**
   * Return true(1) if max lost packets count otherwise false(0).
   * @return boolean.
   */
  bool is_max_lost()
  {
    observe_tx_t observe = read(OBSERVE_TX);
    return (observe.plos_cnt == 15);
  }

  /**
   * Flush receiver/transmitter queue. Check number of lost packages.
   * If maximum returns UINT8_MAX to indicate that the channel is very
   * unreliable or the receiver is not available.
   * @return status.
   */
  uint8_t flush();

  /**
   * Send message from device given source port/pipe, message buffer
   * and size, and destination address. Returns number of bytes sent
   * or negative error code.
   * @param[in] src port/pipe.
   * @param[in] buf pointer to message buffer.
   * @param[in] size of message buffer.
   * @param[in] dest destination address and port.
   * @return number of bytes or negative error code.
   */
  int send(uint16_t src, const void* buf, size_t size, 
	   const Socket::addr_t& dest); 
  
  /**
   * None blocking attempt to receive a message from device. Returns
   * message in given buffer, source node address and port, and
   * destination port or pipe.
   * @param[out] dest destination port or pipe.
   * @param[in/out] buf pointer to message buffer.
   * @param[in] size of message buffer.
   * @param[out] src source address and port.
   * @return number of bytes or negative error code.
   */
  int recv(uint16_t& dest, void* buf, size_t size,
	   Socket::addr_t& src);

  /**
   * Start up the device driver in given initial operation mode, 
   * true(1) receiver otherwise false(0) for transmitter. Return
   * true(1) if successful otherwise false(0).
   * @param[in] mode initial operation mode (zero for transmitter).
   * @return bool
   */
  bool begin(bool mode = false);

  /**
   * Shut down the device driver. Return true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  bool end();

  /**
   * @override
   * Return true(1) if the data is available on the device otherwise
   * false(0). 
   * @return bool
   */
  virtual bool available();

  /**
   * @override
   * Connection-less send message in buffer with given size to
   * destination node.
   * @param[in] s socket.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @param[in] dest address of receiver.
   * @return number of bytes sent or negative error code.
   */
  virtual int send(Socket* s, const void* buf, size_t size, 
		   const Socket::addr_t& dest);

  /**
   * @override
   * Connection-less receive message to buffer with given size.
   * Return source node address and number of bytes received or
   * negative error code. This method is none blocking.
   * @param[in] s socket.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @param[in] dest address of receiver.
   * @return number of bytes received or negative error code.
   */
  virtual int recv(Socket* s, void* buf, size_t size, 
		   Socket::addr_t& src);

  /**
   * @override
   * Create a connection to the given server node. The connection
   * will be establish asynchroniously. Return true(1) if the
   * request was successfully sent otherwise false(0). This method
   * is none blocking. 
   * @param[in] c client.
   * @param[in] server node address.
   * @return bool.
   */
  virtual bool connect(Client* c, const Socket::addr_t& server);

  /**
   * @override
   * Disconnection client from server. Return true(1) if the request
   * was successfully sent otherwise false(0). 
   * @param[in] c client.
   * @return bool.
   */
  virtual bool disconnect(Client* c);

  /**
   * @override
   * Connection-oriented send message in buffer with given size.
   * Returns number of bytes sent or negative error code.
   * This method is none blocking.
   * @param[in] c client.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @return number of bytes sent or negative error code.
   */
  virtual int send(Client* c, const void* buf, size_t size);

  /**
   * @override
   * Connection-oriented receive message to buffer with given size.
   * Return number of bytes received or negative error code. This
   * method is none blocking. 
   * @param[in] c client.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @return number of bytes received or negative error code.
   */
  virtual int recv(Client* c, void* buf, size_t size);

  /**
   * @override
   * Set server in listener mode. Returns true(1) if successful
   * otherwise false(0). This method is none blocking.
   * @param[in] s server.
   * @return bool.
   */
  virtual bool listen(Server* s);

  friend IOStream& operator<<(IOStream& outs, status_t status);
  friend IOStream& operator<<(IOStream& outs, fifo_status_t status);
  friend IOStream& operator<<(IOStream& outs, observe_tx_t observe);
};

/** 
 * Output operator for status field print out.
 * @param[in] outs output stream.
 * @param[in] status value to print.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, NRF24L01P::status_t status);

/** 
 * Output operator for transmitter queue status field print out.
 * @param[in] outs output stream.
 * @param[in] status value to print.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, NRF24L01P::fifo_status_t status);

/** 
 * Output operator for performance statistics field print out.
 * @param[in] outs output stream.
 * @param[in] status value to print.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, NRF24L01P::observe_tx_t observe);

#endif
