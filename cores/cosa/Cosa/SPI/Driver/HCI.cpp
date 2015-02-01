/**
 * @file Cosa/SPI/Driver/HCI.cpp
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

#include "Cosa/SPI/Driver/HCI.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Trace.hh"

uint8_t HCI::DEFAULT_EVNT[DEFAULT_EVNT_MAX];

int 
HCI::read(uint16_t &op, void* args, uint8_t len)
{
  if (!m_available) return (0);
  uint16_t payload = 0;
  bool padding;
  int res = -EFAULT;
  op = 0;
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_READ) == SPI_OP_REPLY) {
    res = -ENOMSG;
    spi.transfer(0);
    spi.transfer(0);
    payload = spi.transfer(0);
    payload = (payload << 8) | spi.transfer(0);
    if (payload >= sizeof(HCI::cmnd_header_t)) {
      cmnd_header_t header;
      spi.read(&header, sizeof(header));
      payload -= sizeof(header);
      res = -EINVAL;
      if (header.type == HCI_TYPE_EVNT || header.type == HCI_TYPE_DATA) {
	if (header.len <= len) {
	  op = header.cmnd;
	  res = header.len;
	  padding = ((res & 1) == 0);
	  spi.read(args, res);
	  if (padding) spi.transfer(0);
	}
      }
    }
  }
  if (res < 0 && payload > 0) {
    for (uint16_t i = 0; i < payload; i++) spi.transfer(0);
  }
  m_available = false;
  spi.end();
  spi.release();
  return (res);
}

int 
HCI::write(uint8_t type, uint16_t op, const void* args, uint8_t len, bool progmem)
{
  bool padding = (len & 1) == 0;
  int payload = sizeof(cmnd_header_t) + len + padding;
  int res = -EFAULT;
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_WRITE) == SPI_OP_REPLY) {
    spi.transfer(payload >> 8);
    spi.transfer(payload);
    spi.transfer(0);
    spi.transfer(0);
    spi.transfer(type);
    spi.transfer(op & 0xff);
    spi.transfer(op >> 8);
    spi.transfer(len);
    if (progmem)
      spi.write_P(args, len);
    else
      spi.write(args, len);
    if (padding) spi.transfer(0);
    res = len;
  }
  spi.end();
  spi.release();
  return (res);
}

int 
HCI::await(uint16_t op, void* args, uint8_t len)
{
  uint32_t start = RTC::millis();
  uint16_t event;
  int res;
  while (1) {
    do {
      while (!m_available && (RTC::since(start) < m_timeout)) yield();
      if (!m_available) return (-ETIME);
      res = read(event, DEFAULT_EVNT, DEFAULT_EVNT_MAX);
    } while (res == -ENOMSG);
    if (res < 0) {
      TRACE(res);
      return (res);
    }
    if (op == event && res == len) {
      memcpy(args, DEFAULT_EVNT, res);
      return (res);
    }
    if (m_event_handler != NULL) 
      m_event_handler->on_event(event, DEFAULT_EVNT, res);
  };
  return (-ENOMSG);
}

int 
HCI::read_data(uint8_t op, void* args, uint8_t args_len,
	       void* data, uint16_t data_len)
{
  if (!m_available) return (0);
  uint16_t payload = 0;
  int res = -EFAULT;
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_READ) == SPI_OP_REPLY) {
    res = -ENOMSG;
    spi.transfer(0);
    spi.transfer(0);
    payload = spi.transfer(0);
    payload = (payload << 8) | spi.transfer(0);
    if (payload >= sizeof(HCI::data_header_t)) {
      data_header_t header;
      spi.read(&header, sizeof(header));
      payload -= sizeof(header);
      res = -EINVAL;
      if (header.type == HCI_TYPE_DATA) {
	if (header.cmnd == op && header.args_len == args_len) {
	  res = header.payload_len - args_len;
	  if ((uint16_t) res <= data_len) {
	    if (args != NULL)
	      spi.read(args, args_len);
	    else
	      for (uint8_t i = 0; i < args_len; i++) spi.transfer(0);
	    spi.read(data, res);
	    if (header.payload_len & 1) spi.transfer(0);
	  }
	}
      }
    }
  }
  if (res < 0) {
    for (uint16_t i = 0; i < payload; i++) spi.transfer(0);
  }
  m_available = false;
  spi.end();
  spi.release();
  return (res);
}

int 
HCI::write_data(uint8_t op, const void* args, uint8_t args_len,
		const void* data, uint16_t data_len, bool progmem)
{
  int len = args_len + data_len;
  int payload = sizeof(data_header_t) + len;
  bool padding = (payload & 1) == 0;
  if (padding) payload += 1;
  int res = -EFAULT;
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_WRITE) == SPI_OP_REPLY) {
    spi.transfer(payload >> 8);
    spi.transfer(payload);
    spi.transfer(0);
    spi.transfer(0);
    spi.transfer(HCI_TYPE_DATA);
    spi.transfer(op);
    spi.transfer(args_len);
    spi.transfer(len);
    spi.transfer(len >> 8);
    spi.write(args, args_len);
    if (progmem)
      spi.write_P(data, data_len);
    else
      spi.write(data, data_len);
    if (padding) spi.transfer(0);
    res = len;
  }
  spi.end();
  spi.release();
  return (res);
}
