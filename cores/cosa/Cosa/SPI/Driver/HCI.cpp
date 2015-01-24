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

#ifdef DEBUG
#include "Cosa/Trace.hh"
#else
#define TRACE(x)
#endif

int 
HCI::await(uint16_t op, void* args, uint8_t len)
{
  uint32_t start = RTC::millis();
  uint16_t event;
  int res;
  do {
    while (!m_available && (RTC::since(start) < m_timeout)) yield();
    if (!m_available) return (-ETIME);
    res = read(event, args, len);
  } while (res == -ENOMSG);
  TRACE(event);
  if ((res < 0) || (op == event)) return (res);
  if (op == 0) return (event);
  return (-ENOMSG);
}

int 
HCI::listen(uint16_t &event, void* args, uint8_t len)
{
  uint32_t start = RTC::millis();
  int res;
  do {
    while (!m_available && (RTC::since(start) < m_timeout)) yield();
    if (!m_available) return (-ETIME);
    res = read(event, args, len);
  } while (res == -ENOMSG);
  return (res);
}

int 
HCI::write(uint8_t type, uint16_t op, void* args, uint8_t len)
{
  bool padding = (len & 1) == 0;
  int payload = sizeof(header_t) + len + padding;
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
    spi.write(args, len);
    if (padding) spi.transfer(0);
    res = len;
  }
  spi.end();
  spi.release();
  return (res);
}

int 
HCI::write_P(uint8_t type, uint16_t op, const void* args, uint8_t len)
{
  bool padding = (len & 1) == 0;
  int payload = sizeof(header_t) + len + padding;
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
    spi.write_P(args, len);
    if (padding) spi.transfer(0);
    res = len;
  }
  spi.end();
  spi.release();
  return (res);
}

int 
HCI::read(uint16_t &op, void* args, uint8_t len)
{
  if (!m_available) return (0);
  uint16_t payload;
  bool padding;
  int res = -EFAULT;
  op = 0;
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_READ) == SPI_OP_REPLY) {
    spi.transfer(0);
    spi.transfer(0);
    payload = spi.transfer(0);
    payload = (payload << 8) | spi.transfer(0);
    if (payload >= sizeof(HCI::header_t)) {
      header_t header;
      spi.read(&header, sizeof(header));
      if (header.type == HCI_TYPE_EVNT) {
	op = header.cmnd;
	if (header.len <= len) {
	  res = header.len;
	  padding = ((res & 1) == 0);
	  spi.read(args, res);
	  if (padding) spi.transfer(0);
	}
	else
	  res = -EINVAL;
      }
    }
    else
      res = -ENOMSG;
  }
  m_available = false;
  spi.end();
  spi.release();
  return (res);
}

int 
HCI::read(uint8_t op, void* ret, uint16_t len)
{
  if (!m_available) return (0);
  uint16_t payload;
  bool padding;
  uint8_t type;
  uint8_t data;
  int res = -EFAULT;
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_READ) == SPI_OP_REPLY) {
    spi.transfer(0);
    spi.transfer(0);
    payload = spi.transfer(0);
    payload = (payload << 8) | spi.transfer(0);
    if (payload >= sizeof(HCI::header_t)) {
      type = spi.transfer(0);
      if (type == HCI_TYPE_DATA) {
	data = spi.transfer(0);
	if (data == op) {
	  uint8_t args = spi.transfer(0);
	  res = spi.transfer(0);
	  res = (spi.transfer(0) << 8) | (res & 0xff);
	  res = res - args;
	  for (uint8_t i = 0; i < args; i++) spi.transfer(0);
	  if ((uint16_t) res <= len) {
	    padding = ((res & 1) == 0);
	    spi.read(ret, res);
	    if (padding) spi.transfer(0);
	  }
	  else
	    res = -EINVAL;
	}
      }
    }
    else
      res = -ENOMSG;
  }
  m_available = false;
  spi.end();
  spi.release();
  return (res);
}
