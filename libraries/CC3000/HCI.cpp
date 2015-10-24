/**
 * @file CC3000/HCI.cpp
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

#include "HCI.hh"
#include "Cosa/RTT.hh"

int
HCI::read(uint16_t &op, void* args, uint8_t len)
{
  // Check that a message is available
  if (!m_available) return (0);

  // Read the SPI message header (big-endian)
  uint16_t payload = 0;
  bool padding;
  int res = EFAULT;
  op = 0;

  // Check header type
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_READ) == SPI_OP_REPLY) {
    res = ENOMSG;
    spi.transfer(0);
    spi.transfer(0);
    payload = spi.transfer(0);
    payload = (payload << 8) | spi.transfer(0);

    // Sanity check the SPI payload
    if (payload >= sizeof(HCI::cmnd_header_t)) {
      cmnd_header_t header;
      spi.read(&header, sizeof(header));
      payload -= sizeof(header);
      res = EINVAL;

      // Check the HCI header type and length
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

  // Remove payload if illegal forma
  if (res < 0) {
    for (uint16_t i = 0; i < payload; i++) spi.transfer(0);
  }

  // Mark the message as read
  m_available = false;
  spi.end();
  spi.release();

  // Return HCI message lenght or negative error code
  return (res);
}

int
HCI::write(uint8_t type, uint16_t op, const void* args, uint8_t len, bool progmem)
{
  // Calculate padding and payload size
  bool padding = (len & 1) == 0;
  int payload = sizeof(cmnd_header_t) + len + padding;
  int res = EFAULT;

  // Check the SPI format and write the SPI in big-endian
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_WRITE) == SPI_OP_REPLY) {
    spi.transfer(payload >> 8);
    spi.transfer(payload);
    spi.transfer(0);
    spi.transfer(0);

    // HCI header in little-endian
    spi.transfer(type);
    spi.transfer(op & 0xff);
    spi.transfer(op >> 8);
    spi.transfer(len);

    // Command block in program memory
    if (progmem)
      spi.write_P(args, len);
    else
      spi.write(args, len);

    // Possible padding for even number of bytes in message
    if (padding) spi.transfer(0);

    // Return size of block written
    res = len;
  }
  spi.end();
  spi.release();

  // Return length of HCI payload
  return (res);
}

int
HCI::await(uint16_t op, void* args, uint8_t len)
{
  uint32_t start = RTT::millis();
  uint16_t event;
  int res;

  // Wait for given event(op) for max timeout setting
  while (1) {

    // Sleep while waiting for a message
    do {
      while (!m_available && (RTT::since(start) < m_timeout)) yield();
      if (!m_available) return (ETIME);
      res = read(event, m_evnt, EVNT_MAX);
    } while (res == ENOMSG);

    // Return on negative error code
    if (res < 0) return (res);

    // Check for event code and event size match
    if (op == event && res == len) {
      if (args != NULL) memcpy(args, m_evnt, res);
      return (res);
    }

    // Otherwise call the message handler
    if (m_event_handler != NULL)
      m_event_handler->on_event(event, m_evnt, res);
  };

  // Should not come here
  return (ENOMSG);
}

int
HCI::read_data(uint8_t op, void* args, uint8_t args_len,
	       void* data, uint16_t data_len)
{
  // Sanity check that a message is available
  if (!m_available) return (0);
  uint16_t payload = 0;
  int res = EFAULT;

  // Check the SPI message header. Read in big-endian
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_READ) == SPI_OP_REPLY) {
    res = ENOMSG;
    spi.transfer(0);
    spi.transfer(0);
    payload = spi.transfer(0);
    payload = (payload << 8) | spi.transfer(0);

    // Santity check the SPI payload size and read HCI header
    if (payload >= sizeof(HCI::data_header_t)) {
      data_header_t header;
      spi.read(&header, sizeof(header));
      payload -= sizeof(header);
      res = EINVAL;

      // Sanity check the HCI format
      if (header.type == HCI_TYPE_DATA) {
	if (header.cmnd == op && header.args_len == args_len) {
	  res = header.payload_len - args_len;
	  if ((uint16_t) res <= data_len) {

	    // Read the event block
	    if (args != NULL)
	      spi.read(args, args_len);
	    else
	      for (uint8_t i = 0; i < args_len; i++) spi.transfer(0);

	    // Read the data block
	    spi.read(data, res);

	    // Read possible padding
	    if (header.payload_len & 1) spi.transfer(0);
	  }
	}
      }
    }
  }

  // Skip payload if a message error was detected
  if (res < 0) {
    for (uint16_t i = 0; i < payload; i++) spi.transfer(0);
  }

  // Mark message read
  m_available = false;
  spi.end();
  spi.release();

  // Return data block size or negative error code
  return (res);
}

int
HCI::write_data(uint8_t op, const void* args, uint8_t args_len,
		const void* data, uint16_t data_len, bool progmem)
{
  // Calculate total and payload size, and padding
  int len = args_len + data_len;
  int payload = sizeof(data_header_t) + len;
  bool padding = (payload & 1) == 0;
  if (padding) payload += 1;
  int res = EFAULT;

  // Sanity check the SPI message header; SPI header in big-endian
  spi.acquire(this);
  spi.begin();
  if (spi.transfer(SPI_OP_WRITE) == SPI_OP_REPLY) {
    spi.transfer(payload >> 8);
    spi.transfer(payload);
    spi.transfer(0);
    spi.transfer(0);

    // HCI header in little-endian
    spi.transfer(HCI_TYPE_DATA);
    spi.transfer(op);
    spi.transfer(args_len);
    spi.transfer(len);
    spi.transfer(len >> 8);
    spi.write(args, args_len);

    // Data block in program memory
    if (progmem)
      spi.write_P(data, data_len);
    else
      spi.write(data, data_len);

    // Check for padding
    if (padding) spi.transfer(0);
    res = len;
  }
  spi.end();
  spi.release();

  // Return total number of bytes written or a negative error code
  return (res);
}
