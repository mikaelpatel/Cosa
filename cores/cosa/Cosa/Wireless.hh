/**
 * @file Cosa/Wireless.hh
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

#ifndef __COSA_WIRELESS_HH__
#define __COSA_WIRELESS_HH__

#include "Cosa/Power.hh"

/**
 * Cosa Common Wireless devise driver interface.
 */
class Wireless {
public:
  class Driver {
  public:
    /** Network address */
    struct addr_t {
      int16_t network;
      uint8_t device;
      addr_t(int16_t net, uint8_t dev) 
      {
	network = net;
	device = dev;
      }
    };
    
  protected:
    /** Current channel */
    uint8_t m_channel;
    /** Current network and device address */
    addr_t m_addr;
    /** Message available */
    volatile bool m_avail;
    /** Sleep mode on wait */
    uint8_t m_mode;

  public:
    /**
     * Construct Wireless Driver with given network and device address.
     * @param[in] network address.
     * @param[in] device address.
     */
    Driver(int16_t network, uint8_t device) :
      m_channel(0),
      m_addr(network, device),
      m_avail(false),
      m_mode(SLEEP_MODE_IDLE)
    {}

    /**
     * Set power sleep mode during wait.
     * @param[in] mode of sleep.
     */
    void set_sleep(uint8_t mode)
    {
      m_mode = mode;
    }

    /**
     * Set network and device address. Do not use the broadcast
     * address(0). Should be used before calling begin().
     * @param[in] net network address.
     * @param[in] dev device address.
     */
    void set_address(int16_t net, uint8_t dev)
    {
      m_addr.network = net;
      m_addr.device = dev;
    }

    /**
     * Set device transmission channel. Should be used before calling
     * begin(). 
     * @param[in] channel.
     */
    void set_channel(uint8_t channel)
    {
      m_channel = channel;
    }

    /**
     * Start the Wireless device driver. Return true(1) if successful
     * otherwise false(0).
     * @param[in] config configuration vector (default NULL)
     * @return bool
     */
    virtual bool begin(const void* config = NULL) = 0;

    /**
     * Shut down the device driver. Return true(1) if successful
     * otherwise false(0).
     * @return bool
     */
    virtual bool end()
    {
      return (true);
    }
    
    /**
     * Set device in power down mode. 
     */
    virtual void powerdown() {}

    /**
     * Set device in wakeup on radio mode. 
     */
    virtual void wakeup_on_radio() {}

    /**
     * Return true(1) if a message is available otherwise false(0).
     * @return bool
     */
    virtual bool available()
    {
      return (m_avail);
    }

    /**
     * Return true(1) if there is room to send on the device otherwise
     * false(0).  
     * @return bool
     */
    virtual bool room()
    {
      return (true);
    }
  
    /**
     * Send message in given buffer, with given number of bytes. Returns
     * number of bytes sent. Returns error code(-1) if number of bytes
     * is greater than PAYLOAD_MAX. Return error code(-2) if fails to
     * set transmit mode.  
     * @param[in] dest destination network address.
     * @param[in] buf buffer to transmit.
     * @param[in] len number of bytes in buffer.
     * @return number of bytes send or negative error code.
     */
    virtual int send(uint8_t dest, const void* buf, size_t len) = 0;

    /**
     * Receive message and store into given buffer with given maximum
     * length. The source network address is returned in the parameter src.
     * Returns error code(-2) if no message is available and/or a
     * timeout occured. Returns error code(-1) if the buffer size if to
     * small for incoming message or if the receiver fifo has overflowed. 
     * Otherwise the actual number of received bytes is returned
     * @param[out] src source network address.
     * @param[in] buf buffer to store incoming message.
     * @param[in] len maximum number of bytes to receive.
     * @param[in] ms maximum time out period.
     * @return number of bytes received or negative error code.
     */
    virtual int recv(uint8_t& src, void* buf, size_t len, uint32_t ms = 0L) = 0;
  };
};
#endif
