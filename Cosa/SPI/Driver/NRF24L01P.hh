/**
 * @file Cosa/SPI/Driver/NRF24L01P.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * SPI driver for the nRF24L01 Single Chip 2.4GHz Transceiver.
 * For details see nRF24L01+ Product Specification (Rev. 1.0)
 * http://www.nordicsemi.com/kor/nordic/download_resource/8765/2/17776224
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_SPI_DRIVER_NRF24L01P_HH__
#define __COSA_SPI_DRIVER_NRF24L01P_HH__

#include "Cosa/Pins.hh"
#include "Cosa/Event.hh"
#include "Cosa/SPI.hh"

class NRF24L01P : private SPI::Driver {
private:
  /**
   * SPI Commands (See chap. 8.3.1, tab. 20, pp. 51)
   */
  enum Command {
    R_REGISTER = 0x00,		// Read command and status register (5 bit addr)
    W_REGISTER = 0x20,		// Write command and status register (5 bit addr)
    REG_MASK = 0x1F,		// Mask register address
    R_RX_PAYLOAD = 0x61,	// Read RX payload
    W_TX_PAYLOAD = 0xA0,	// Write TX payload
    FLUSH_TX = 0xE1,		// Flush TX FIFO
    FLUSH_RX = 0xE2,		// Flush RX FIFO
    REUSE_TX_PL = 0xE3,		// Reuse last transmitted payload
    R_RX_PL_WID = 0x60,		// Read RX payload width
    W_ACK_PAYLOAD = 0xA8,	// Write TX payload with ACK (3 bit addr)
    PIPE_MASK = 0x07,		// Mask pipe address
    W_ACK_PAYLOAD_NOACK = 0xB0,	// Disable AUTOACK on this specific packet
    NOP = 0xFF			// No operation, return status
  } __attribute__((packed));

  /**
   * Transmission state
   */
  uint8_t m_status;
  uint8_t m_channel;

  /**
   * Slave select pin (default is pin 10)
   */
  OutputPin m_csn;

  /**
   * Chip enable activity RX/TX select pin (default is pin 9)
   */
  OutputPin m_ce;

  /**
   * Chip interrupt pin (default is pin 2)
   */
  class IRQPin : public ExternalInterruptPin {
    friend class NRF24L01P;
  private:
    NRF24L01P* m_nrf;
  public:
    IRQPin(Board::ExternalInterruptPin pin, Mode mode, NRF24L01P* nrf) : 
      ExternalInterruptPin(pin, mode),
      m_nrf(nrf)
    {}
    virtual void on_interrupt(uint16_t arg = 0);
  };
  IRQPin m_irq;

public:
  /**
   * Construct NRF transceiver with given channel and pin numbers 
   * for SPI slave select, activity enable and interrupt. Default
   * in parenthesis (Standard/Mega Arduino).
   * @param[in] channel number (default 64).
   * @param[in] csn spi slave select pin number (default D10/D53).
   * @param[in] ce chip enable activates pin number (default D9/D48).
   * @param[in] irq interrupt pin number (default EXT0/EXT4).
   */
#if defined(__ARDUINO_MEGA__)
  NRF24L01P(uint8_t channel = 64, 
	    Board::DigitalPin csn = Board::D53, 
	    Board::DigitalPin ce = Board::D48, 
	    Board::ExternalInterruptPin irq = Board::EXT4);
#else
  NRF24L01P(uint8_t channel = 64, 
	    Board::DigitalPin csn = Board::D10, 
	    Board::DigitalPin ce = Board::D9, 
	    Board::ExternalInterruptPin irq = Board::EXT0);
#endif

  /**
   * NRF transceiver states (See chap. 6.1.1, fig. 4, pp. 22)
   */
  enum State {
    POWER_DOWN_STATE = 0,
    STANDBY_STATE = 1,
    RX_STATE = 2,
    TX_STATE = 3
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
    RX_ADDR_P0 = 0x0A,		// Receive address data pipe 0
    RX_ADDR_P1 = 0x0B,		// - data pipe 1
    RX_ADDR_P2 = 0x0C,		// - data pipe 2
    RX_ADDR_P3 = 0x0D,		// - data pipe 3
    RX_ADDR_P4 = 0x0E,		// - data pipe 4
    RX_ADDR_P5 = 0x0F,		// - data pipe 5
    TX_ADDR = 0x10,		// Transmit address
    RX_PW_P0 = 0x11,		// Number of bytes in RX payload in data pipe 0
    RX_PW_P1 = 0x12,		// - data pipe 1
    RX_PW_P2 = 0x13,		// - data pipe 2
    RX_PW_P3 = 0x14,		// - data pipe 3
    RX_PW_P4 = 0x15,		// - data pipe 4
    RX_PW_P5 = 0x16,		// - data pipe 5
    FIFO_STATUS = 0x17,		// FIFO status register
    DYNPD = 0x1C,		// Enable dynamic payload length
    FEATURE = 0x1D		// Feature register
  } __attribute__((packed));

  /**
   * Register CONFIG bitfields
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
   * Register EN_AA bitfields
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
   * Register EN_RXADDR bitfields
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
   * Register SETUP_AW bitfields
   */
  enum {
    AW = 0,			// RX/TX address field width (bits 2)
    AW3BYTES = 1,		// 3 bytes
    AW4BYTES = 2,		// 4 bytes
    AW5BYTES = 3		// 5 bytes
  } __attribute__((packed));

  /**
   * Register SETUP_RETR bitfields
   */
  enum {
    ARD = 4,			// Auto retransmit delay (bits 4)
    				// - delay * 250 us (250..4000 us)
    ARC = 0			// Auto retransmit count (bits 4)
    				// - retransmit count (0..15)
  } __attribute__((packed));

  /**
   * Register RF_SETUP bitfields
   */
  enum {
    CONT_WAVE = 7,	    	// Continuous carrier transmit
    RF_DR_LOW = 5,		// Set RF data rate to 250 kbps
    PLL_LOCK = 4,		// Force PLL lock signal
    RF_DR_HIGH = 3,		// Air data bitrate (2 Mbps)
    RF_PWR = 1			// Set RF output power in TX mode (bits 2)
  } __attribute__((packed));

  /**
   * Transmission rates RF_DR_LOW/RF_DR_HIGH values
   */
  enum {
    RF_DR_1MBPS = 0,
    RF_DR_2MBPS = _BV(RF_DR_HIGH),
    RF_DR_250KBPS = _BV(RF_DR_LOW)
  } __attribute__((packed));

  /**
   * Output power RF_PWR values
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
    RX_P_NO = 1,		// Data pipe number for available payload (bits 3)
    RX_P_NO_MASK = 0x0e,	// Mask pipe number
    TX_FIFO_FULL = 0		// TX FIFO full flag
  } __attribute__((packed));

  /**
   * Register OBSERVE_TX bitfields
   */
  enum  {
    PLOS_CNT = 4,		// Count lost packets (bits 4)
    ARC_CNT = 0		        // Count retransmitted packets (bits 4)
  } __attribute__((packed));

  /**
   * Register FIFO_STATUS bitfields
   */
  enum {
    TX_REUSE = 6,		// Reuse last transmitted data packat
    TX_FULL = 5,		// TX FIFO full flag
    TX_EMPTY = 4,		// TX FIFO empty flag
    RX_FULL = 1,		// RX FIFO full flag
    RX_EMPTY = 0,		// RX FIFO empty flag
  } __attribute__((packed));

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
    PAYLOAD_MAX = 32,           // Max size of payload
    AW_MAX = 5,                 // Max address width in bytes
  } __attribute__((packed));

  /**
   * Start interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin()
  {
    return (spi.begin(SPI::DIV4_CLOCK, 0, SPI::MSB_FIRST));
  }

  /**
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  bool end()
  {
    return (spi.end());
  }

  /**
   * Read command and status registers. Issue R_REGISTER command.
   * @param[in] reg register address.
   * @return register value.
   */
  uint8_t read(Register reg);

  /**
   * Write command and status registers. Issue W_REGISTER command.
   * @param[in] reg register address.
   * @param[in] data new setting.
   * @return status.
   */
  uint8_t write(Register reg, uint8_t data);

  /**
   * Write command and status registers. Issue W_REGISTER command and
   * multiple values.
   * @param[in] reg register address.
   * @param[in] buffer data storage.
   * @param[in] count number of bytes to write.
   * @return status.
   */
  uint8_t write(Register reg, const void* buffer, uint8_t count);

  /**
   * Get status of latest operation. Issue NOP command.
   * @return status.
   */
  uint8_t get_status()
  {
    SPI_transaction(m_csn) {
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
   * @return status.
   */
  void set_powerup_mode();

  /**
   * Set transmitter mode and use the given address for transmitter and
   * receiver address for pipe 0 (acknowledgements).
   * @param[in] addr address string.
   * @param[in] width address width (default AW_MAX).
   */
  void set_transmitter_mode(const void* addr, uint8_t width = AW_MAX);

  /**
   * Set receiver mode and use the given address for receiver address
   * for pipe 1. Last byte in address plus one is used for pipe 2, etc.
   * @param[in] addr address string.
   * @param[in] width addres width.
   */
  void set_receiver_mode(const void* addr, uint8_t width = AW_MAX);

  /**
   * Set standby mode. 
   * @return status.
   */
  void set_standby_mode();

  /**
   * Set power down. Turn off radio and go into low power mode. 
   * @return status.
   */
  void set_powerdown_mode();

  /**
   * Return true(1) if data is available, otherwise false(0).
   * @return boolean.
   */
  bool is_available();

  /**
   * Read received payload into given buffer. Returns number of bytes
   * payload received.
   * @param[in] buffer data storage.
   * @param[out] pipe sender.
   * @return number of bytes received.
   */
  uint8_t recv(void* buffer, uint8_t* pipe = 0);

  /**
   * Return true(1) if device is ready, otherwise false(0).
   * @return boolean.
   */
  bool is_ready();

  /**
   * Return true(1) if max retransmit attempts otherwise false(0).
   * @return boolean.
   */
  bool is_max_retransmit();

  /**
   * Return true(1) if max lost packets count otherwise false(0).
   * @return boolean.
   */
  bool is_max_lost();

  /**
   * Send acknowledgement payload from given buffer. Return number of 
   * transmitted bytes.
   * @param[in] buffer data storage.
   * @param[in] count number of bytes to send [1..PAYLOAD_MAX].
   * @param[in] pipe number of pipe write acknowledge payload.
   * @return number of transmitted bytes.
   */
  uint8_t ack(const void* buffer, uint8_t count, uint8_t pipe);

  /**
   * Send acknowledgement payload from given buffer in program memory. 
   * Return number of transmitted bytes.
   * @param[in] buffer data storage.
   * @param[in] count number of bytes to send [1..PAYLOAD_MAX].
   * @param[in] pipe number of pipe write acknowledge payload.
   * @return number of transmitted bytes.
   */
  uint8_t ack_P(const void* buffer, uint8_t count, uint8_t pipe);

  /**
   * Send payload from given buffer. Return number of transmitted
   * bytes. 
   * @param[in] buffer data storage.
   * @param[in] count number of bytes to send [1..PAYLOAD_MAX].
   * @return number of transmitted bytes.
   */
  uint8_t send(const void* buffer, uint8_t count);

  /**
   * Send payload from given buffer in program memory. Return 
   * number of transmitted bytes.
   * @param[in] buffer data storage in program memory.
   * @param[in] count number of bytes to send [1..PAYLOAD_MAX].
   * @return number of transmitted bytes.
   */
  uint8_t send_P(const void* buffer, uint8_t count);

  /**
   * Flush receiver/transmitter queue. Check number of lost packages.
   * If maximum returns UINT8_MAX to indicate that the channel is very
   * unreliable or the receiver is not available.
   * @return status.
   */
  uint8_t flush();

private:
  /**
   * Transceiver state
   */
  State m_state;
};

#endif
