/**
 * @file CC3000/HCI.hh
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

#ifndef COSA_CC3000_HCI_HH
#define COSA_CC3000_HCI_HH

#include "Cosa/SPI.hh"
#include "Cosa/ExternalInterrupt.hh"

/**
 * Host Control Interface (HCI) Protocol for SPI. Abstraction of the
 * communication with CC3000 WiFi module. Handles command issue, data
 * write, and reply and data read. Writes and reads HCI blocks in
 * SPI frame. SPI frames are in big-endian, HCI frames in
 * little-endian, except som data which are in network order
 * (big-endian).
 *
 * @section References
 * 1. CC3000 Protocol, http://processors.wiki.ti.com/index.php/CC3000_Protocol
 */
class HCI : public SPI::Driver {
public:

  /**
   * HCI Event handler for unsolicited events.
   */
  class Event {
  public:
    class Handler {
    public:
      /**
       * @override{HCI::Event::Handler}
       * Unsolicited event callback.
       * @param[in] event operation code.
       * @param[in] args pointer to argument block.
       * @param[in] len number of bytes in argument block.
       */
      virtual void on_event(uint16_t event, void* args, size_t len) = 0;
    };
  };

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
    m_timeout(DEFAULT_TIMEOUT),
    m_event_handler(NULL)
  {
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

  /**
   * Write given HCI type operation and arguments. Returns argument
   * length or negative error code.
   * @param[in] type HCI message type.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block.
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int write(uint8_t type, uint16_t op, const void* args, uint8_t len)
  {
    return (write(type, op, args, len, false));
  }

  /**
   * Write given HCI type operation and arguments in program
   * memory. Returns argument length or negative error code.
   * @param[in] type HCI message type.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block in program memory.
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int write_P(uint8_t type, uint16_t op, const void* args, uint8_t len)
  {
    return (write(type, op, args, len, true));
  }

  /**
   * Write given HCI type operation and arguments. Returns argument
   * length or negative error code.
   * @param[in] type HCI message type.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block (program memory).
   * @param[in] len number of bytes in argument block.
   * @param[in] progmem argment block in program memory.
   * @return argument length or negative error code.
   */
  int write(uint8_t type, uint16_t op, const void* args, uint8_t len,
	    bool progmem);

  /**
   * Issue given HCI command message and arguments. Returns argument
   * length or negative error code.
   * @param[in] op HCI operation command.
   * @param[in] args pointer to argument block.
   * @param[in] len number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int issue(uint16_t op, const void* args = NULL, uint8_t len = 0)
  {
    return (write(HCI_TYPE_CMND, op, args, len, false));
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
    return (write(HCI_TYPE_CMND, op, args, len, true));
  }

  /**
   * Await HCI event and arguments. Returns argument length
   * or negative error code. The given argument block must be able to
   * hold incoming event message. A default event block is used when
   * passer NULL.
   * @param[in] op HCI event code required.
   * @param[in] args pointer to argument block.
   * @param[in] len max number of bytes in argument block.
   * @return argument length or negative error code.
   */
  int await(uint16_t op, void* args = NULL, uint8_t len = 0);

  /**
   * Write data with given data operation code, argument block and
   * data payload. Returns number of bytes written or negative error
   * code.
   * @param[in] op data operation code.
   * @param[in] args pointer to argument block.
   * @param[in] args_len number of bytes in argument block.
   * @param[in] data pointer to data block
   * @param[in] data_len number of bytes in data block.
   * @return number of bytes written or negative error code.
   */
  int write_data(uint8_t op, const void* args, uint8_t args_len,
		 const void* data, uint16_t data_len)
  {
    return (write_data(op, args, args_len, data, data_len, false));
  }

  /**
   * Write data with given data operation code, argument block and
   * data payload in program memory. Returns number of bytes written
   * or negative error code.
   * @param[in] op data operation code.
   * @param[in] args pointer to argument block.
   * @param[in] args_len number of bytes in argument block.
   * @param[in] data pointer to data block in program memory.
   * @param[in] data_len number of bytes in data block.
   * @return number of bytes written or negative error code.
   */
  int write_data_P(uint8_t op, const void* args, uint8_t args_len,
		   const void* data, uint16_t data_len)
  {
    return (write_data(op, args, args_len, data, data_len, true));
  }

  /**
   * Write data with given data operation code, argument block and
   * data payload in given memory source. Returns number of bytes written
   * or negative error code.
   * @param[in] op data operation code.
   * @param[in] args pointer to argument block.
   * @param[in] args_len number of bytes in argument block.
   * @param[in] data pointer to data block.
   * @param[in] data_len number of bytes in data block.
   * @param[in] progmem flag data block in program memory.
   * @return number of bytes written or negative error code.
   */
  int write_data(uint8_t op, const void* args, uint8_t args_len,
		 const void* data, uint16_t data_len, bool progmem);

  /**
   * Issue HCI read data command message and arguments. Returns payload
   * length or negative error code.
   * @param[in] op HCI data operation command.
   * @param[in] args pointer to command argument block.
   * @param[in] args_len number of bytes in command argument block.
   * @param[in] data pointer to data block.
   * @param[in] data_len number of bytes in data block.
   * @return argument length or negative error code.
   */
  int read_data(uint8_t op, void* args, uint8_t args_len,
		void* data, uint16_t data_len);

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
   * Return true(1) if a packet is available otherwise false(0).
   * @return bool.
   */
  bool is_available()
  {
    return (m_available);
  }

  /**
   * Set event service handler. Called by service() for incoming
   * events.
   * @param[in] handler.
   */
  void event_handler(Event::Handler* handler)
  {
    m_event_handler = handler;
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
     * @override{Interrupt::Handler}
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
   * HCI Command Header.
   */
  struct cmnd_header_t {
    uint8_t type;		//!< HCI Message Type.
    uint16_t cmnd;		//!< HCI Operation Code (little-endian).
    uint8_t len;		//!< HCI Arguments Length.
  };

  /**
   * HCI Data Header.
   */
  struct data_header_t {
    uint8_t type;		//!< HCI Message Type.
    uint8_t cmnd;		//!< HCI Data Operation Code.
    uint8_t args_len;		//!< HCI Arguments Length.
    uint16_t payload_len;	//!< HCI Payload Length.
  };

  /**
   * HCI Message Types.
   */
  enum {
    HCI_TYPE_CMND = 0x01,	//!< HCI Command (SPI_OP_WRITE only).
    HCI_TYPE_DATA = 0x02,	//!< HCI Data (both SPI_OP_WRITE/READ).
    HCI_TYPE_PATCH = 0x03,	//!< HCI Patch (SPI_OP_WRITE only).
    HCI_TYPE_EVNT = 0x04	//!< HCI Event (SPI_OP_READ only).
  };

  /** Interrupt request handler. */
  IRQPin m_irq;

  /** Request flag. */
  volatile bool m_available;

  /** Reply timeout in milli-seconds. */
  uint16_t m_timeout;

  /** Event handler for unsolicited events. */
  Event::Handler* m_event_handler;

  /** Size of default event block. */
  static const uint8_t EVNT_MAX = 64;

  /** Default event block. */
  uint8_t m_evnt[EVNT_MAX];
};
#endif
