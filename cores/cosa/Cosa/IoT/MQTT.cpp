/**
 * @file Cosa/IoT/MQTT.cpp
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

#include "Cosa/IoT/MQTT.hh"
#include "Cosa/INET/DNS.hh"
#include "Cosa/Watchdog.hh"

const char MQTT::PROTOCOL[] __PROGMEM = { 
  0, 6, 				// Length(6)
  'M', 'Q', 'I', 's', 'd', 'p',		// Name("MQIsdp")
  3					// Version(3)
};

int 
MQTT::Client::write(uint8_t cmd, uint16_t length, uint16_t id)
{
  int res = sizeof(uint8_t);
  if (length > 0x7f) {
    length = ((length & 0xff80) << 1) | (length & 0x7f) | (0x80);
    res += sizeof(uint8_t);
  }
  m_sock->write(&cmd, sizeof(cmd));
  m_sock->write(&length, res);
  if (id != 0) {
    m_sock->write(&id, sizeof(id));
    res += sizeof(uint16_t);
  }
  return (res + sizeof(uint8_t));
}

int 
MQTT::Client::write(const void* buf, size_t count)
{
  return (m_sock->write(buf, count));
}

int 
MQTT::Client::write_P(const void* buf, size_t count)
{
  return (m_sock->write_P(buf, count));
}

int 
MQTT::Client::puts_P(str_P s)
{
  uint16_t length = strlen_P(s);
  int res = length + sizeof(length);
  length = hton((int16_t) length);
  m_sock->write(&length, sizeof(length));
  if (length != 0) m_sock->puts_P(s);
  return (res);
}

int 
MQTT::Client::read(void* buf, size_t count, uint32_t ms)
{
  uint32_t start = Watchdog::millis();
  int res;
  while (((res = m_sock->available()) == 0) &&
	 ((ms == 0L) || (Watchdog::millis() - start < ms)))
    yield();
  if (res == 0) return (-2);
  return (m_sock->read(buf, count));
}

int
MQTT::Client::flush()
{
  return (m_sock->flush());
}

bool 
MQTT::Client::begin(Socket* sock)
{
  if (sock == NULL) return (false);
  m_sock = sock;
  return (true);
}

bool 
MQTT::Client::end()
{
  if (m_sock == NULL) return (false);
  m_sock->close();
  m_sock = NULL;
  return (true);
}

int 
MQTT::Client::connect(const char* hostname,
		      const char* identifier, 
		      uint16_t keep_alive, 
		      uint8_t flag, 
		      ...)
{
  uint16_t qos = 0;
  va_list args;

  // Connect to the server. Check for timeout
  int res = m_sock->connect(hostname, PORT);
  if (res != 0) return (-1);
  while ((res = m_sock->is_connected()) == 0) yield();
  if (res == 0) res = -2;
  if (res < 0) return (res);

  // Calculate length of variable payload
  size_t length = sizeof(PROTOCOL) + sizeof(flag) + sizeof(keep_alive);
  length += strlen_P(identifier) + sizeof(uint16_t);
  va_start(args, flag);
  if (flag & WILL_FLAG) {
    const char* topic = va_arg(args, const char*);
    const char* will = va_arg(args, const char*);
    qos = va_arg(args, uint16_t);
    length += strlen_P(topic) + sizeof(uint16_t);
    length += strlen_P(will) + sizeof(uint16_t);
    flag |= ((qos & QOS_MASK) << WILL_QOS_POS);
  }
  if (flag & USER_NAME_FLAG) {
    const char* user = va_arg(args, const char*);
    length += strlen_P(user) + sizeof(uint16_t);
  }
  if (flag & PASSWORD_FLAG) {
    const char* password = va_arg(args, const char*);
    length += strlen_P(password) + sizeof(uint16_t);
  }
  va_end(args);

  // Write command, length, protocol, flags, keep-alive and identifier
  keep_alive = hton((int16_t) keep_alive);
  write(CONNECT, length);
  write_P(PROTOCOL, sizeof(PROTOCOL));
  write(&flag, sizeof(flag));
  write(&keep_alive, sizeof(keep_alive));
  puts_P((str_P) identifier);

  // Write variable parameters
  va_start(args, flag);
  if (flag & WILL_FLAG) {
    str_P topic = va_arg(args, str_P);
    str_P will = va_arg(args, str_P);
    qos = va_arg(args, uint16_t);
    puts_P(topic);
    puts_P(will);
  }
  if (flag & USER_NAME_FLAG) {
    str_P user = va_arg(args, str_P);
    puts_P(user);
  }
  if (flag & PASSWORD_FLAG) {
    str_P password = va_arg(args, str_P);
    puts_P(password);
  }
  va_end(args);
  res = flush();
  if (res < 0) return (-1);
  
  // Wait for response; CONNACK or timeout
  struct {
    uint8_t cmd;
    uint8_t length;
    uint8_t reserved;
    uint8_t code;
  } response;
  res = read(&response, sizeof(response));
  if (res != sizeof(response)) return (-2);
  if ((response.cmd != CONNACK) || (response.length != 2)) return (-3);
  res = -response.code;
  return (res);
}

int 
MQTT::Client::disconnect()
{
  uint16_t cmd = DISCONNECT;
  write(&cmd, sizeof(cmd));
  return (m_sock->disconnect());
}

int 
MQTT::Client::publish(str_P topic, const void* buf, size_t count,
		      QoS_t qos, bool retain,
		      bool progmem)

{
  uint16_t length = strlen_P(topic) + sizeof(uint16_t);
  uint16_t id = 0;

  // Check if message identity is required
  if (qos > FIRE_AND_FORGET) {
    length += sizeof(uint16_t);
    id = hton((int16_t) m_mid++);
    if (m_mid == 0) m_mid = 1;
  }
  length += count;

  // Write message; command, length, topic, {id}, payload
  write(PUBLISH | (qos << MESSAGE_QOS_POS) | (retain & RETAIN), length);
  puts_P(topic);
  if (qos > FIRE_AND_FORGET) write(&id, sizeof(id));
  if (progmem) write_P(buf, count); else write(buf, count);
  int res = flush();
  if (res < 0) return (-1);

  // Wait for response. Depends on requested quality of service
  struct {
    uint8_t cmd;
    uint8_t length;
    uint16_t id;
  } response;

  // Check acknowledged and assured delivery
  switch (qos) {
  case FIRE_AND_FORGET: 
    return (0);
  case ACKNOWLEDGED_DELIVERY:
    res = read(&response, sizeof(response));
    if (res != sizeof(response)) return (-2);
    if ((response.cmd != PUBACK) 
	|| (response.length != sizeof(response.id))
	|| (response.id != id)) return (-3);
    return (0);
  case ASSURED_DELIVERY:
    res = read(&response, sizeof(response));
    if (res != sizeof(response)) return (-2);
    if ((response.cmd != PUBREC) 
	|| (response.length != sizeof(response.id))
	|| (response.id != id)) return (-3);
    response.cmd = PUBREL;
    write(&response, sizeof(response));
    res = flush();
    if (res < 0) return (-1);
    res = read(&response, sizeof(response));
    if (res != sizeof(response)) return (-2);
    if ((response.cmd != PUBCOMP) 
	|| (response.length != sizeof(response.id))
	|| (response.id != id)) return (-4);
    return (0);
  }
  return (-1);
}

int
MQTT::Client::subscribe(str_P topic, QoS_t qos)
{
  // Calculate length of variable payload; id, topic, qos
  size_t length = sizeof(uint16_t);
  length += strlen_P(topic) + sizeof(uint16_t) + sizeof(uint8_t);
  uint16_t id = hton((int16_t) m_mid++);
  if (m_mid == 0) m_mid = 1;

  // Write command, length, message identity and topic/qos
  write(SUBSCRIBE, length, id);
  puts_P(topic);
  write(&qos, sizeof(uint8_t));
  int res = flush();

  // Wait for response; SUBACK or timeout
  struct {
    uint8_t cmd;
    uint8_t length;
    uint16_t id;
    uint8_t qos;
  } response;
  res = read(&response, sizeof(response));
  if (res != sizeof(response)) return (-2);
  if ((response.cmd != SUBACK) 
      || (response.length != 3)
      || (response.id != id)
      || (response.qos != qos)) return (-3);
  return (0);
}

int 
MQTT::Client::unsubscribe(str_P topic)
{
  // Calculate length of variable payload
  size_t length = sizeof(uint16_t);
  length += strlen_P(topic) + sizeof(uint16_t);
  uint16_t id = hton((int16_t) m_mid++);
  if (m_mid == 0) m_mid = 1;

  // Write command, length, message identity and topic
  write(UNSUBSCRIBE, length, id);
  puts_P(topic);
  int res = flush();
  if (res < 0) return (-1);
  
  // Wait for response; UNSUBACK or timeout
  struct {
    uint8_t cmd;
    uint8_t length;
    uint16_t id;
  } response;
  res = read(&response, sizeof(response));
  if (res != sizeof(response)) return (-2);
  if ((response.cmd != UNSUBACK) 
      || (response.length != 2)
      || (response.id != id)) return (-3);
  return (0);
}

int
MQTT::Client::service(uint32_t ms)
{
  struct {
    uint8_t cmd;
    uint8_t length;
  } request;

  // Read request (publish). Check for timeout
  int res = read(&request, sizeof(request), ms);
  if (res != sizeof(request)) return (res);
  uint16_t length = request.length;
  if (request.length > 128) {
    uint8_t msb;
    res = read(&msb, sizeof(msb));
    length |= (msb << 7);
  }

  // Check that it is a publish
  if ((request.cmd & MESSAGE_TYPE_MASK) != PUBLISH) return (-1);
  uint8_t qos = ((request.cmd & MESSAGE_QOS_MASK) >> MESSAGE_QOS_POS);

  // Read topic length and string
  uint16_t count;
  res = read(&count, sizeof(count));
  if (res != sizeof(count)) return (-2);
  count = ntoh((int16_t) count);
  char topic[count + 1];
  res = read(topic, count);
  if (res != (int) count) return (-2);
  topic[count] = 0;
  length -= count + sizeof(count);

  // Read message identity (for higher quality of service)
  uint16_t id = 0;
  if (qos != FIRE_AND_FORGET) {
    res = read(&id, sizeof(id));
    if (res != sizeof(id)) return (-2);
    length -= sizeof(id);
  }

  // Read payload and call on_publish handler
  uint8_t payload[length + 1];
  res = read(payload, length);
  if (res != (int) length) return (-2);
  payload[length] = 0;

  // Write response message(s)
  struct {
    uint8_t cmd;
    uint8_t length;
    uint16_t id;
  } response;
  response.length = sizeof(response.id);
  response.id = id;

  switch (qos) {
  case FIRE_AND_FORGET: 
    on_publish(topic, payload, length);
    return (0);
  case ACKNOWLEDGED_DELIVERY:
    response.cmd = PUBACK;
    res = write(&response, sizeof(response));
    res = flush();
    if (res < 0) return (-3);
    on_publish(topic, payload, length);
    return (0);
  case ASSURED_DELIVERY:
    response.cmd = PUBREC;
    res = write(&response, sizeof(response));
    res = flush();
    if (res < 0) return (-4);
    res = read(&response, sizeof(response));
    if (res != sizeof(response)) return (-2);
    if ((response.cmd != PUBREL) 
	|| (response.length != sizeof(response.id))
	|| (response.id != id)) return (-5);
    response.cmd = PUBCOMP;
    write(&response, sizeof(response));
    res = flush();
    if (res < 0) return (-6);
    on_publish(topic, payload, length);
    return (0);
  }
  return (-1);
}

void 
MQTT::Client::on_publish(char* topic, void* buf, size_t count)
{
  UNUSED(topic);
  UNUSED(buf);
  UNUSED(count);
}
