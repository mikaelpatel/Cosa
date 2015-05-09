/**
 * @file Cosa/Rete.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_RETE_HH
#define COSA_RETE_HH

#include "Cosa/Types.h"
#include "Cosa/Periodic.hh"
#include "Cosa/Wireless.hh"
#include "Cosa/Registry.hh"

/**
 * Cosa Small Network Management and Data Distribution Protocol.
 * Maps application data with a registry and makes it available
 * through the wireless interface. Two classes are defined; one classs
 * for network management and another for wireless sensor elements.
 * The network management protocol is a scaled down version of
 * SNMP. It allows get and put of values in the registry and
 * dispatching of actions. The data distribution protocol is a micro
 * version of DDS with only a publish message to broadcast registry
 * updates.
 *
 * @section References
 * 1. OMG Data Distribution Service Portal, http://portals.omg.org/dds/
 * 2. Simple Network Management Protocol,
 * http://en.wikipedia.org/wiki/Simple_Network_Management_Protocol
 */
class Rete {
public:
  /**
   * A Rete::Device is the base-class of wireless sensor nodes. The
   * default behaviour is a periodic function that will handle power
   * up and down.
   */
  class Device : public Periodic {
  public:
    /**
     * Construct device protocol handler.
     * @param[in] root item list.
     * @param[in] dev wireless device.
     */
    Device(Registry* reg, Wireless::Driver* dev, uint16_t ms) :
      Periodic(ms),
      m_dev(dev),
      m_reg(reg),
      m_tid(0)
    {}

    /**
     * Update the registry item with the given value and broadcast.
     * Return transaction identity or negative error code.
     * @param[in] product identity.
     * @param[in] path in registry.
     * @param[in] len length of path.
     * @param[in] buf buffer with new value to assign.
     * @param[in] size number of bytes in buffer.
     * @return request identity of negative error code.
     */
    int publish(uint16_t product, const uint8_t* path, size_t len,
		const void* buf, size_t size)
    {
      iovec_t vec[4];
      iovec_t* vp = vec;
      iovec_arg(vp, &product, sizeof(product));
      iovec_arg(vp, path, len);
      iovec_arg(vp, buf, size);
      iovec_end(vp);
      int res = m_dev->broadcast(PUBLISH, vec);
      return (res > 0 ? m_tid++ : res);
    }

    /**
     * @override Periodic
     * The Rete device periodic function; on wakeup data is measured
     * and published. The device should listen for manager requests
     * for a short period before power down.
     */
    virtual void run();

  protected:
    /** Wireless device. */
    Wireless::Driver* m_dev;

    /** Registry root. **/
    Registry* m_reg;

    /** Next transaction identity (15b, positive number only). */
    int16_t m_tid;
  };

  class Manager {
  public:
    /**
     * Construct manager protocol handler.
     * @param[in] dev wireless device.
     */
    Manager(Wireless::Driver* dev) :
      m_dev(dev),
      m_tid(0)
    {}

    /**
     * Send a registry get value request to given destination device and
     * given path. Returns request identity or negative error code.
     * @param[in] dest destination device.
     * @param[in] path in registry.
     * @param[in] len length of path.
     * @return request identity of negative error code.
     */
    int get_request(uint8_t dest, const uint8_t* path, size_t len)
    {
      iovec_t vec[3];
      iovec_t* vp = vec;
      iovec_arg(vp, &m_tid, sizeof(m_tid));
      iovec_arg(vp, path, len);
      iovec_end(vp);
      int res = m_dev->send(dest, GET_REQUEST, vec);
      return (res > 0 ? m_tid++ : res);
    }

    /**
     * Send a registry put value request to given destination device and
     * given registry path and value in given buffer. Returns request
     * identity or negative error code.
     * @param[in] dest destination device.
     * @param[in] path in registry.
     * @param[in] len length of path.
     * @param[in] buf buffer with new value to assign.
     * @param[in] size number of bytes in buffer.
     * @return request identity of negative error code.
     */
    int put_request(uint8_t dest, const uint8_t* path, size_t len,
		    const void* buf, size_t size)
    {
      iovec_t vec[4];
      iovec_t* vp = vec;
      iovec_arg(vp, &m_tid, sizeof(m_tid));
      iovec_arg(vp, path, len);
      iovec_arg(vp, buf, size);
      iovec_end(vp);
      int res = m_dev->send(dest, PUT_REQUEST, vec);
      return (res > 0 ? m_tid++ : res);
    }

    /**
     * Send a registry action apply request to given destination device and
     * given registry path and arguments in given buffer. Returns request
     * identity or negative error code.
     * @param[in] dest destination device.
     * @param[in] path in registry.
     * @param[in] len length of path.
     * @param[in] args buffer with action arguments.
     * @param[in] size number of bytes in buffer.
     * @return request identity of negative error code.
     */
    int apply_request(uint8_t dest, const uint8_t* path, size_t len,
		      const void* args, size_t size)
    {
      iovec_t vec[4];
      iovec_t* vp = vec;
      iovec_arg(vp, &m_tid, sizeof(m_tid));
      iovec_arg(vp, path, len);
      iovec_arg(vp, args, size);
      iovec_end(vp);
      int res = m_dev->send(dest, APPLY_REQUEST, vec);
      return (res > 0 ? m_tid++ : res);
    }

    /**
     * Listen for incoming responses for max given number of
     * milli-seconds.
     * @param[in] ms milli-seconds.
     */
    int listen(uint16_t ms);

  protected:
    /** Wireless device. */
    Wireless::Driver* m_dev;

    /** Next transaction identity (15b, positive number only). */
    int16_t m_tid;
  };

protected:
  /**
   * Message types.
   */
  enum {
    RETE_BASE = 128,		//!< Base message number.
    PUBLISH = RETE_BASE,	//!< Publish registry update.
    GET_REQUEST,		//!< Get registry item value request.
    GET_RESPONSE,		//!< - response with value.
    PUT_REQUEST,		//!< Put registry item value request.
    PUT_RESPONSE,		//!< - response with status.
    APPLY_REQUEST,		//!< Apply registry action request.
    APPLY_RESPONSE,		//!< - reponse with result.
  } __attribute__((packed));
};

#endif

