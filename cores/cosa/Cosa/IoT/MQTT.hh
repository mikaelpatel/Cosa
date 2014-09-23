/**
 * @file Cosa/IoT/MQTT.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#ifndef COSA_IOT_MQTT_HH
#define COSA_IOT_MQTT_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"

/**
 * MQTT V3.1 Protocol client implementation.
 * 
 * @section Reference
 * 1. MQTT V3.1 Protocol Specification, 
 *    Copyright (c) 1999-2010, Eurotech, IBM.
 */
class MQTT {
public:
  /** Quality of service levels on publish (pp. 5-6). */
  enum QoS_t {
    FIRE_AND_FORGET = 0x00,		//!< At most once; Fire and forget.
    ACKNOWLEDGED_DELIVERY = 0x01, 	//!< At least once; Acknowledged delivery.
    ASSURED_DELIVERY = 0x2,		//!< Exactly once; Assured delivery.
  } __attribute__((packed));

  /** MQTT Server default TCP/IP port. */
  static const uint16_t PORT = 1883;

  /**
   * MQTT Client class; allow access to MQTT server.
   */
  class Client {
  public:
    /**
     * Default constructor; initiate client state.
     */
    Client() : 
      m_sock(NULL), 
      m_mid(1) 
    {}

    /**
     * Default destructor.
     */
    ~Client() 
    { 
      end(); 
    }

    /**
     * Start MQTT client with given socket. The socket will be used for 
     * all access (DNS and MQTT server) until closed with the member
     * function end() or the default destructor. Returns true if
     * successful otherwise false. 
     * @param[in] sock connection-less socket (TCP).
     * @return bool.
     */
    bool begin(Socket* sock);

    /** 
     * Stop MQTT client and close socket. Returns true if successful
     * otherwise false.
     * @return bool.
     */
    bool end();

    /** Connect flags (pp. 8-9). */
    enum {			
      WILL_FLAG = 0x04,		//!< Will flag.
      USER_NAME_FLAG = 0x80,	//!< User name flag.
      PASSWORD_FLAG = 0x40,	//!< Password flag.
      WILL_RETAIN = 0x20,	//!< Will retain.
      CLEAN_SESSION = 0x02	//!< Clean information.
    } __attribute__((packed));

    /**
     * Connect using given hostname with client identifier, given
     * keep alive timeout in seconds, flags and optional
     * parameters. Returns zero if successful otherwise negative error
     * code. 
     * @param[in] hostname server name.
     * @param[in] identifier for client (program memory string).
     * @param[in] keep_alive time limit between messages (Default 600).
     * @param[in] flag connect flags (Default none).
     * @param[in] will topic (Optional program memory string).
     * @param[in] message will (Optional program memory string).
     * @param[in] user name (Optional program memory string).
     * @param[in] password string (Optional program memory string).
     * @return zero if successful otherwise negative error code.
     */
    int connect(const char* hostname,
		const char* identifier, 
		uint16_t keep_alive = 600, 
		uint8_t flag = 0, 
		...);

    /**
     * Disconnect from MQTT server. Returns zero if successful
     * otherwise negative error code.
     * @return zero if successful otherwise negative error code.
     */
    int disconnect();

    /**
     * Publish the value in buffer to the given topic with the given
     * QoS and retain flag. Returns zero if successful otherwise
     * negative error code.
     * @param[in] topic string (program memory).
     * @param[in] buf buffer pointer (data or program memory).
     * @param[in] count number of bytes in buffer.
     * @param[in] qos quality of service.
     * @param[in] retain require server to maintain value.
     * @param[in] progmem flag if payload buffer in program memory.
     * @return zero if successful otherwise negative error code.
     */
    int publish(str_P topic, const void* buf, size_t count, 
		QoS_t qos, bool retain, bool progmem);

    /**
     * Publish the value in buffer to the given topic with the given
     * QoS and retain flag. Returns zero if successful otherwise
     * negative error code.
     * @param[in] topic string (program memory).
     * @param[in] buf buffer pointer.
     * @param[in] count number of bytes in buffer.
     * @param[in] qos quality of service (Default FIRE_AND_FORGET).
     * @param[in] retain require server to maintain value (Default false).
     * @return zero if successful otherwise negative error code.
     */
    int publish(str_P topic, const void* buf, size_t count, 
		QoS_t qos = FIRE_AND_FORGET, 
		bool retain = false)
      __attribute__((always_inline))
    {
      return (publish(topic, buf, count, qos, retain, false));
    }

    /**
     * Publish the value in buffer to the given topic with the given
     * QoS and retain flag. Returns zero if successful otherwise
     * negative error code.
     * @param[in] topic string (program memory).
     * @param[in] buf buffer pointer (program memory).
     * @param[in] count number of bytes in buffer.
     * @param[in] qos quality of service (Default FIRE_AND_FORGET).
     * @param[in] retain require server to maintain value (Default false).
     * @return zero if successful otherwise negative error code.
     */
    int publish_P(str_P topic, const void* buf, size_t count, 
		  QoS_t qos = FIRE_AND_FORGET, 
		  bool retain = false)
    {
      return (publish(topic, buf, count, qos, retain, true));
    }

    /**
     * Subscribe to value changes on the given topics and requested
     * quality of service. Returns zero if successful otherwise
     * negative error code.
     * @param[in] topic string (program memory).
     * @param[in] qos requested quality of service.
     * @return zero if successful otherwise negative error code.
     */
    int subscribe(str_P topic, QoS_t qos = FIRE_AND_FORGET);

    /**
     * Unsubscribe on the given topic. Returns zero if successful
     * otherwise negative error code. 
     * @param[in] topic string (program memory).
     * @return zero if successful otherwise negative error code.
     */
    int unsubscribe(str_P topic);

    /**
     * Service the MQTT client. Check for publish messages. Decode and
     * calls virtual member function on_publish(). Returns zero if
     * successful otherwise negative error code. 
     * @param[in] ms timeout period, milli-seconds (Default BLOCK).
     * @return zero if successful otherwise negative error code.
     */
    int service(uint32_t ms = 0L);
    
    /**
     * @override MQTT::Client
     * Called by service when received a publish message. 
     * @param[in] topic string.
     * @param[in] buf buffer with topic value.
     * @param[in] count number of bytes in buffer.
     * @return zero if successful otherwise negative error code.
     */
    virtual void on_publish(char* topic, void* buf, size_t count);

  protected:
    /** Connection-oriented socket. */
    Socket* m_sock;

    /** Message sequence number (1..UINT16_MAX). */
    uint16_t m_mid;

    // Support member functions
    int write(uint8_t cmd, uint16_t length, uint16_t id = 0);
    int write(const void* buf, size_t count);
    int write_P(const void* buf, size_t count);
    int puts_P(str_P s);
    int read(void* buf, size_t count, uint32_t ms = 3000L);
    int flush();
  };
protected:
  /** Message format: Fixed header message type and flags (pp. 4-7). */
  enum {
    CONNECT = 0x10,		//!< Client request to connect to Server.
      CONNACK = 0x20,		//!< Connect acknowledgement.
    PUBLISH = 0x30,		//!< Publish message.
      PUBACK = 0x40,		//!< Publish acknowledgement.
      PUBREC = 0x50,		//!< Publish received (assured delivery part 1).
      PUBREL = 0x62,		//!< Publish release (assured delivery part 2).
      PUBCOMP = 0x70,		//!< Publish complete (assured delivery part 3).
    SUBSCRIBE = 0x82,		//!< Client subscribe request (acknowledge delivery).
      SUBACK = 0x90,		//!< Subscribe acknowledgement.
    UNSUBSCRIBE = 0xa2,		//!< Client unsubscribe request (acknowledge delivery).
      UNSUBACK = 0xb0,		//!< Unsubscribe acknowledgement.
    PINGREQ = 0xc0,		//!< PING request.
      PINGRESP = 0xd0,		//!< PING response.
    DISCONNECT = 0xe0,		//!< Client is disconnecting.
    DUP = 0x08,			//!< Attempt to redeliver message.
    RETAIN = 0x01,		//!< Server should hold on to the message.
    MESSAGE_QOS_POS = 1,	//!< Quality of service (bit position).
    MESSAGE_QOS_MASK = 0x06,	//!< Quality of service (bit mask).
    MESSAGE_TYPE_MASK = 0xf0,	//!< Mask message type.
    MESSAGE_FLAG_MASK = 0x0f	//!< Mask message flags.
  } __attribute__((packed));

  /** Bit position for will quality of service. */
  static const uint8_t WILL_QOS_POS = 3;

  /** Bit mask for quality of service value. */
  static const uint8_t QOS_MASK = 0x03;

  /** MQTT connect magic header. */
  static const char PROTOCOL[] PROGMEM;
};

#endif
