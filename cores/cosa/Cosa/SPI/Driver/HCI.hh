/**
 * @file Cosa/SPI/Driver/HCI.hh
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

#ifndef COSA_SPI_DRIVER_HCI_HH
#define COSA_SPI_DRIVER_HCI_HH

#include "Cosa/SPI.hh"
#include "Cosa/ExternalInterrupt.hh"

/**
 * Host Control Interface (HCI) Protocol for SPI. Abstraction of the
 * communication with CC3000 WiFi module. Handles command issue, data
 * write, and reply and data read. 
 * 
 * @section References
 * 1. CC3000 Protocol, http://processors.wiki.ti.com/index.php/CC3000_Protocol
 */
class HCI : public SPI::Driver {
public:
  /** Default timeout on command/data reply (in ms). */
  static const uint16_t DEFAULT_TIMEOUT = 3000;

  /**
   * Construct SPI based HCI driver with given chip select and
   * interrupt request pin. May also configure with SPI clock rate.
   * @param[in] cs chip select pin.
   * @param[in] irq interrupt request pin.
   * @param[in] rate of communication with device.
   */
  HCI(Board::DigitalPin cs, 
      Board::ExternalInterruptPin irq, 
      SPI::Clock rate = SPI::DEFAULT_CLOCK) :
    SPI::Driver(cs, SPI::ACTIVE_LOW, rate, 1, SPI::MSB_ORDER, &m_irq),
    m_irq(irq, this),
    m_available(false),
    m_timeout(DEFAULT_TIMEOUT)
  {
  }

  /**
   * Issue given HCI command message and arguments. Returns argument
   * length or negative error code.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block.
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int issue(uint16_t op, void* args = NULL, uint8_t len = 0)
  {
    return (write(HCI_TYPE_CMND, op, args, len));
  }

  /**
   * Issue given HCI command message and arguments in program memory
   * block. Returns argument length or negative error code.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block.
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int issue_P(uint16_t op, const void* args, uint8_t len)
  {
    return (write_P(HCI_TYPE_CMND, op, args, len));
  }

  /**
   * Await HCI event and arguments. Returns argument length
   * or negative error code. The given argument block must be able to
   * hold incoming packet. 
   * @param[in] op HCI event code required.
   * @param[in] args pointer to argument block.
   * @param[in] len max number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int await(uint16_t op, void* args = NULL, uint8_t len = 0);
  
  /**
   * Listen for HCI event and arguments. Returns argument length
   * or negative error code. The given argument block must be able to
   * hold incoming packet. 
   * @param[out] event HCI event code received.
   * @param[in] args pointer to argument block.
   * @param[in] len max number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int listen(uint16_t &event, void* args = NULL, uint8_t len = 0);

  /**
   * Write HCI data operation and arguments. Returns argument
   * length or negative error code.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block.
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int write(uint16_t op, void* args, uint8_t len)
  {
    return (write(HCI_TYPE_DATA, op, args, len));
  }

  /**
   * Read HCI operation and arguments. Returns argument length
   * or negative error code. The given argument block must be able to
   * hold incoming packet. 
   * @param[out] op HCI operation command.
   * @param[in] args pointer to argument block.
   * @param[in] len max number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int read(uint16_t &op, void* args, uint8_t len);
  int read(uint8_t op, void* ret, uint16_t len);
  
  /**
   * Write given HCI type operation and arguments. Returns argument
   * length or negative error code.
   * @param[in] type HCI message type.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block.
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int write(uint8_t type, uint16_t op, void* args, uint8_t len);

  /**
   * Write given HCI type operation and arguments in program memory
   * block. Returns argument length or negative error code.
   * @param[in] type HCI message type.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block (program memory).
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int write_P(uint8_t type, uint16_t op, const void* args, uint8_t len);

  /**
   * Enable incoming HCI packets (DATA/EVNT).
   */
  void enable()
  {
    m_irq.enable();
  }

  /**
   * Disable incoming HCI packets (DATA/EVNT).
   */
  void disable()
  {
    m_irq.disable();
  }
  
  /**
   * Return true(1) if a packet is available.
   * @return bool.
   */
  bool is_available()
  {
    return (m_available);
  }

protected:
  /**
   * Handler for interrupt pin. Service interrupt on incoming HCI
   * messages (DATA/EVNT). 
   */
  class IRQPin : public ExternalInterrupt {
    friend class HCI;
  public:
    /**
     * Construct interrupt pin handler for HCI message receive
     * interrupt. Service interrupt on falling mode. Force active
     * pullup.
     * @param[in] pin external interrupt pin.
     * @param[in] hci device.
     */
    IRQPin(Board::ExternalInterruptPin pin, HCI* hci) : 
      ExternalInterrupt(pin, ExternalInterrupt::ON_FALLING_MODE, true),
      m_hci(hci)
    {}
    
    /**
     * @override Interrupt::Handler
     * Signal that a packet has is ready and may be read.
     * @param[in] arg (not used).
     */
    virtual void on_interrupt(uint16_t arg)
    {
      UNUSED(arg);
      m_hci->m_available = true;
    }

  private:
    HCI* m_hci;			//!< HCI module to signal.
  };

  /**
   * SPI Header Operation Code.
   */
  enum {
    SPI_OP_WRITE = 0x01,	//!< SPI Write(Payload Length,0).
    SPI_OP_REPLY = 0x02,	//!< SPI Reply(device ready).
    SPI_OP_READ = 0x03		//!< SPI Read(0,Payload Length).
  };

  /**
   * HCI Header.
   */
  struct header_t {
    uint8_t type;		//!< HCI Message Type.
    uint16_t cmnd;		//!< HCI Operation Code (little-endian).
    uint8_t len;		//!< HCI Arguments Length.
  };

  /**
   * HCI Message Types.
   */
  enum {
    HCI_TYPE_CMND = 0x01,	//!< HCI Command (SPI_OP_WRITE).
    HCI_TYPE_DATA = 0x02,	//!< HCI Data (SPI_OP_WRITE/READ).
    HCI_TYPE_PATCH = 0x03,	//!< HCI Patch (SPI_OP_WRITE).
    HCI_TYPE_EVNT = 0x04	//!< HCI Event (SPI_OP_READ).
  };

  /** Interrupt request handler. */
  IRQPin m_irq;
  
  /** Request flag. */
  volatile bool m_available;

  /** Reply timeout in milli-seconds. */
  uint16_t m_timeout;
};
#endif
