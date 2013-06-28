/**
 * @file Cosa/Socket.hh
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

#ifndef __COSA_SOCKET_HH__
#define __COSA_SOCKET_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

// Forward declaration for Socket::Device
class Client;
class Server;

/**
 * Abstract connectionless communication. Base class for Client and
 * Server which provide connection-oriented communication. Contains
 * the definition and support class for network device drivers.
 */
class Socket {
public:
  /** Start of dynamic port section */
  static const uint16_t DYNAMIC_PORT = 49152U;

  /**
   * Socket address data type; node address and port number.
   * Socket device driver must be able to handle addressing.
   */
  struct addr_t {
    uint32_t addr;
    uint16_t port;
  };
  
  /**
   * Abstract socket device driver interface. Supports basic management
   * of socket-port mapping, attach, detach and lookup, and access of
   * client and server private/protected state. 
   */
  class Device {
    friend class Socket;
  protected:
    /** Device network address */
    uint32_t m_addr;

    /** Maximum number of sockets in mapping */
    static const uint8_t SOCKET_MAX = 16;

    /** Mapping from device to sockets. Socket holds port information */
    Socket* m_socket[SOCKET_MAX];
    
    /**
     * Attach given socket to device. Return socket binding or negative
     * error code.
     * @param[in] s socket to attach.
     * @return binding or negative error code.
     */
    int8_t attach(Socket* s);

    /**
     * Detach given socket from device. 
     * @param[in] s socket to detach.
     */
    void detach(Socket* s);

    /**
     * Lookup socket in device map given port number. Returns socket 
     * reference or null(0).
     * @param[in] port number.
     * @return socket or null(0).
     */
    Socket* lookup(uint16_t port);

    /**
     * Mark given client as connected to the given destination.
     * @param[in] client.
     * @param[in] dest.
     */
    void set_connected(Client* client, const Socket::addr_t& dest);

    /**
     * Mark given client as disconnected
     * @param[in] client.
     */
    void set_disconnected(Client* client);

    /**
     * Bind client to given port.
     * @param[in] client.
     * @param[in] port.
     */
    void set_port(Client* client, uint16_t port);

    /**
     * Get destination address of given client.
     * @param[in] client.
     * @return node address.
     */
    Socket::addr_t get_dest_address(Client* client);
    
  public:
    /**
     * Construct socket driver with given network address (32b)
     * @param[in] addr node address.
     */
    Device(uint32_t addr) :
      m_addr(addr)
    {
      memset(m_socket, 0, sizeof(m_socket));
    }

    /**
     * Construct socket driver with given network address (32b) in
     * EEPROM.
     * @param[in] addr node address (EEMEM variable).
     */
    Device(const uint32_t* addr)
    {
      m_addr = (uint32_t) pgm_read_dword(addr);
      memset(m_socket, 0, sizeof(m_socket));
    }

    /**
     * Construct socket driver with given network address (4x8b)
     * @param[in] x0 MSB node address byte.
     * @param[in] x1 node address.
     * @param[in] x2 node address.
     * @param[in] x3 node address.
     */
    Device(uint8_t x0, uint8_t x1, uint8_t x2, uint8_t x3)
    {
      univ32_t addr;
      addr.as_uint8[0] = x0;
      addr.as_uint8[1] = x1;
      addr.as_uint8[2] = x2;
      addr.as_uint8[3] = x3;
      m_addr = addr.as_uint32;
      memset(m_socket, 0, sizeof(m_socket));
    }
    
    /**
     * @override
     * Return true(1) if the data is available on the device otherwise
     * false(0). 
     * @return bool
     */
    virtual bool available() = 0;

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
		     const Socket::addr_t& dest) = 0;

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
		     Socket::addr_t& src) = 0;

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
    virtual bool connect(Client* c, const Socket::addr_t& server) = 0;

    /**
     * @override
     * Disconnection client from server. Return true(1) if the request
     * was successfully sent otherwise false(0). 
     * @param[in] c client.
     * @return bool.
     */
    virtual bool disconnect(Client* c) = 0;

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
    virtual int send(Client* c, const void* buf, size_t size) = 0;

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
    virtual int recv(Client* c, void* buf, size_t size) = 0;

    /**
     * @override
     * Set server in listener mode. Returns true(1) if successful
     * otherwise false(0). 
     * @param[in] s server.
     * @return bool.
     */
    virtual bool listen(Server* s) = 0;
  };

protected:
  /** Socket device */
  Device* m_dev;

  /** Socket port, device holds address */
  uint16_t m_port;
  
public:
  /**
   * Create connectionless socket. Allows sending and receiving of
   * datagrams.
   * @param[in] dev socket device.
   * @param[in] port socket port.
   */
  Socket(Device* dev, uint16_t port) : 
    m_dev(dev),
    m_port(port)
  {
    if (is_connectionless()) m_dev->attach(this);
  }

  /**
   * Get socket network address.
   * @return address.
   */
  addr_t get_address()
  {
    addr_t src;
    src.addr = m_dev->m_addr;
    src.port = m_port;
    return (src);
  }

  /**
   * Get socket port number.
   * @return port number.
   */
  uint16_t get_port()
  {
    return (m_port);
  }

  /**
   * @override
   * Return true if the socket is connection less.
   * @return bool.
   */
  virtual bool is_connectionless() 
  { 
    return (true); 
  }

  /**
   * Return true if the socket is connection oriented.
   * @return bool.
   */
  bool is_connection_oriented() 
  {
    return (!is_connectionless()); 
  }

  /**
   * @override
   * Return true if the socket is a server/listener.
   * @return bool.
   */
  virtual bool is_server() 
  { 
    return (false); 
  }

  /**
   * Attach socket for incoming messages, on_recv(). By default the
   * socket is always attached when constructed. May be used to allow
   * periodic servicing of incoming messages after detach.
   */
  virtual void attach() 
  {
    m_dev->attach(this);
  }

  /**
   * Detach socket and disable handling of incoming messages. This may
   * be used to allow outgoing messages only and allow low power standby
   * device driver mode.
   */
  virtual void detach() 
  {
    m_dev->detach(this);
  }

  /**
   * Send message (buffer of given size) to given destination node
   * address and port. Return number of bytes sent if successful
   * otherwise negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @param[in] dest address and port of destination.
   * @return number of bytes sent or negative error code.
   */
  int send(const void* buf, size_t size, const addr_t& dest)
  {
    if (!is_connectionless()) return (-1);
    return (m_dev->send(this, buf, size, dest));
  }

  /**
   * Attempt to receive a message into the given buffer with given max
   * size. Return length of message and source address if successful
   * otherwise negative error code or zero for no message available.
   * @param[in/out] buf buffer pointer.
   * @param[in] size of buffer.
   * @param[out] src address and port of destination.
   * @return number of bytes sent or negative error code.
   */
  int recv(void* buf, size_t size, addr_t& src)
  {
    if (!is_connectionless()) return (-1);
    return (m_dev->recv(this, buf, size, src));
  }
  
  /**
   * @override
   * Callback when a message is received. Default is an empty methods.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @param[in] src address and port of destination.
   */
  virtual void on_recv(const void* buf, size_t size, const addr_t& src) 
  {
  }
};

/**
 * Write given network address and port to given output stream. The
 * address is written in dot notation with port. 
 * @param[in] outs output stream.
 * @param[in] src source node address and port.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, const Socket::addr_t& src);

/**
 * Connection-oriented communication socket client. 
 */
class Client : public Socket {
  friend class Socket::Device;
private:
  /**
   * Attach is not allowed for a client socket. This is handled with
   * connect() instead.
   */
  virtual void attach() {}

  /**
   * Detach is not allowed for a client socket. This is handled with
   * disconnect() instead.
   */
  virtual void detach() {}

protected:
  /** Client states */
  enum State {
    DISCONNECTED_STATE,
    CONNECTING_STATE,
    CONNECTED_STATE,
    DISCONNECTING_STATE
  } __attribute__((packed));

  /** Server node address, defined on connect */
  addr_t m_server;
  
  /** Destination node address, defined when connected */
  addr_t m_dest;

  /** Current client state */
  State m_state;
  
  /**
   * @override
   * Callback when a message is received. Forward to client version where
   * src address is already known through the binding.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @param[in] src address and port of destination.
   */
  virtual void on_recv(const void* buf, size_t size, const addr_t& src) 
  {
    on_recv(buf, size);
  }

public:
  /**
   * Create client on given device. Further initialization is
   * performed on connect() and when the connection has been
   * established. 
   * @param[in] dev socket device driver.
   */
  Client(Socket::Device* dev) : 
    Socket(dev, 0),
    m_state(DISCONNECTED_STATE)
  {}

  /**
   * @override
   * Returns false(0) as this is not connectionless communication.
   * @return bool
   */
  virtual bool is_connectionless() 
  { 
    return (false); 
  }

  /**
   * Issue a connection request to given server node and port. Returns
   * true(1) if the request was successful otherwise false(0). This
   * method is none blocking. 
   * @return bool
   */
  bool connect(const Socket::addr_t& server)
  {
    m_server = server;
    m_state = CONNECTING_STATE;
    return (m_dev->connect(this, server));
  }

  /**
   * Disconnect from the current server connection. Returns true(1) if
   * the request was successful otherwise false(0). 
   * @return bool
   */
  bool disconnect()
  {
    m_state = DISCONNECTING_STATE;
    return (m_dev->disconnect(this));
  }

  /**
   * Returns true(1) if connected has been established otherwise
   * false(0). 
   * @return bool
   */
  bool is_connected() 
  { 
    return (m_state == CONNECTED_STATE); 
  }

  /**
   * Returns true(1) if disconnected has been established otherwise
   * false(0). 
   * @return bool
   */
  bool is_disconnected() { 
    return (m_state == DISCONNECTED_STATE); 
  }
  
  /**
   * Send message (buffer of given size). Return number of bytes sent
   * if successful otherwise negative error code. The socket must be
   * connected. This method is none blocking. 
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   * @return number of bytes sent or negative error code.
   */
  int send(const void* buf, size_t size)
  {
    if (m_state != CONNECTED_STATE) return (-1);
    return (m_dev->send(this, buf, size));
  }

  /**
   * Attempt to receive a message into the given buffer with given max
   * size. Return length of message and source address if successful
   * otherwise negative error code or zero for no message available.
   * This method is none blocking. 
   * @param[in/out] buf buffer pointer.
   * @param[in] size of buffer.
   * @return number of bytes sent or negative error code.
   */
  int recv(void* buf, size_t size)
  {
    if (m_state != CONNECTED_STATE) return (-1);
    return (m_dev->recv(this, buf, size));
  }

  /**
   * @override
   * Client callback when a connection is established and the socket 
   * is ready for communication.
   */
  virtual void on_connected() 
  {
  }

  /**
   * @override
   * Callback when a message is received. Default is an empty methods.
   * @param[in] buf buffer pointer.
   * @param[in] size of buffer.
   */
  virtual void on_recv(const void* buf, size_t size) 
  {
  }

  /**
   * @override
   * Client callback when disconnected by server. Default is an empty 
   * method.
   */
  virtual void on_disconnected() 
  {
  }
};

/**
 * Connection-oriented communication socket server. 
 */
class Server : public Socket {
public:
  /**
   * Service handler; Server side handling of a Client connection.
   */
  class Service : public Client {
  public:
    /**
     * Create service handler on given device.
     * @param[in] dev socket device driver.
     */
    Service(Socket::Device* dev) : Client(dev) {}
  };

  /**
   * Create server with given network device driver and listen port.
   * @param[in] dev device driver.
   * @param[in] port number.
   */
  Server(Socket::Device* dev, uint16_t port) : 
    Socket(dev, port) 
  {
  }

  /**
   * @override
   * Return true(1) as this class is a server.
   * @return bool
   */
  virtual bool is_server() 
  { 
    return (true);
  }

  /**
   * Initiate server as listener. Returns true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  bool listen()
  {
    return (m_dev->listen(this));
  }

  /**
   * @override
   * Server callback on connect request from given source node. Return
   * accepted service handler otherwise null(0) to reject. 
   * Must be defined by sub-class. 
   * @param[in] src source node address.
   * @return service handler or null(0)
   */
  virtual Service* on_connect_request(Socket::addr_t& src) = 0;

  /**
   * @override
   * Server callback on disconnect request for given service
   * handler. Server should notify client and reclaim client structure.
   * Must be defined by sub-class. 
   * @param[in] client.
   * @return client
   */
  virtual void on_disconnect_request(Service* service) = 0;
};
#endif
