/**
 * @file S25FL127S.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "S25FL127S.hh"

bool
S25FL127S::begin()
{
  // Check that the device is ready
  if (!is_ready()) return (false);

  // Read identification
  spi.acquire(this);
    spi.begin();
      spi.transfer(READ_ID);
      spi.transfer(0);
      spi.transfer(0);
      spi.transfer(0);
      uint8_t manufacturer = spi.transfer(0);
      uint8_t device = spi.transfer(0);
    spi.end();
  spi.release();

  // And check
  return (manufacturer == MANUFACTURER && device == DEVICE);
}

bool
S25FL127S::is_ready()
{
  // Read Status Register 1
  spi.acquire(this);
    spi.begin();
      spi.transfer(RDSR1);
      m_status = spi.transfer(0);
    spi.end();
  spi.release();

  // Return Write-In-Progress is off
  return (!m_status.WIP);
}

int
S25FL127S::read(void* dest, uint32_t src, size_t size)
{
  // Use READ with 24-bit address; Big-endian
  uint8_t* sp = (uint8_t*) &src;
  int res = (int) size;
  spi.acquire(this);
    spi.begin();
      spi.transfer(READ);
      spi.transfer(sp[2]);
      spi.transfer(sp[1]);
      spi.transfer(sp[0]);
      spi.read(dest, size);
    spi.end();
  spi.release();

  // Return number of bytes read
  return (res);
}

int
S25FL127S::erase(uint32_t dest, uint8_t size)
{
  uint8_t op;
  switch (size) {
  case 4: op = P4E; break;
  case 64: op = SER; break;
  case 255: op = BER; break;
  default: return (EINVAL);
  }
  spi.acquire(this);
    // Write enable before page erase.
    spi.begin();
      spi.transfer(WREN);
    spi.end();
    // Use erase(P4E/SER/BER) with possible 24-bit address; Big-endian
    uint8_t* dp = (uint8_t*) &dest;
    spi.begin();
      spi.transfer(op);
      if (op != BER) {
	spi.transfer(dp[2]);
	spi.transfer(dp[1]);
	spi.transfer(dp[0]);
      }
    spi.end();
  spi.release();

  // Wait for completion
  while (!is_ready()) yield();

  // Return error code(-1) if erase error otherwise zero
  return (m_status.E_ERR ? EFAULT : 0);
}

int
S25FL127S::write(uint32_t dest, const void* src, size_t size)
{
  // Check for zero buffer size
  if (UNLIKELY(size == 0)) return (0);

  // Set up destination and source pointers
  uint8_t* dp = (uint8_t*) &dest;
  uint8_t* sp = (uint8_t*) src;
  int res = (int) size;

  // Calculate block size of first program
  size_t count = PAGE_MAX - (dest & PAGE_MASK);
  if (UNLIKELY(count > size)) count = size;

  while (1) {
    spi.acquire(this);
      // Write enable before program
      spi.begin();
        spi.transfer(WREN);
      spi.end();
      // Use PP with 24-bit address; Big-endian
      spi.begin();
        spi.transfer(PP);
	spi.transfer(dp[2]);
	spi.transfer(dp[1]);
	spi.transfer(dp[0]);
	spi.write(sp, count);
      spi.end();
    spi.release();

    // Wait for completion
    while (!is_ready()) yield();

    // Check for program error
    if (UNLIKELY(m_status.P_ERR)) return (EFAULT);

    // Step to next page
    size -= count;
    if (UNLIKELY(size == 0)) break;
    dest += count;
    sp += count;
    count = (size > PAGE_MAX ? PAGE_MAX : size);
  }

  // Return number of bytes programmed
  return (res);
}

int
S25FL127S::write_P(uint32_t dest, const void* src, size_t size)
{
  // Check for zero buffer size
  if (UNLIKELY(size == 0)) return (0);

  // Set up destination and source pointers
  uint8_t* dp = (uint8_t*) &dest;
  uint8_t* sp = (uint8_t*) src;
  int res = (int) size;

  // Calculate block size of first program
  size_t count = PAGE_MAX - (dest & PAGE_MASK);
  if (UNLIKELY(count > size)) count = size;

  while (1) {
    spi.acquire(this);
      // Write enable before program
      spi.begin();
        spi.transfer(WREN);
      spi.end();
      // Use PP with 24-bit address; Big-endian
      spi.begin();
        spi.transfer(PP);
	spi.transfer(dp[2]);
	spi.transfer(dp[1]);
	spi.transfer(dp[0]);
	spi.write_P(sp, count);
      spi.end();
    spi.release();

    // Wait for completion
    while (!is_ready()) yield();

    // Check for program error
    if (UNLIKELY(m_status.P_ERR)) return (EFAULT);

    // Step to next page
    size -= count;
    if (UNLIKELY(size == 0)) break;
    dest += count;
    sp += count;
    count = (size > PAGE_MAX ? PAGE_MAX : size);
  }

  // Return number of bytes programmed
  return (res);
}

uint8_t
S25FL127S::issue(Command cmd)
{
  spi.acquire(this);
    spi.begin();
      spi.transfer(cmd);
      uint8_t res = spi.transfer(0);
    spi.end();
  spi.release();
  return (res);
}
