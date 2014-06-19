/**
 * @file Cosa/SPI/Driver/S25FL127S.cpp
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

#include "Cosa/SPI/Driver/S25FL127S.hh"

bool 
S25FL127S::is_ready()
{
  // Read Status Register#1
  spi.begin(this);
  spi.transfer(RDSR1);
  m_status.as_uint8 = spi.transfer(0);
  spi.end();

  // Return Write-In-Progress is off
  return (m_status.WIP == 0);
}

int 
S25FL127S::read(void* dest, uint32_t src, size_t size)
{
  // Use READ with 24-bit address; Big-endian
  uint8_t* sp = (uint8_t*) &src;
  spi.begin(this);
  spi.transfer(READ);
  spi.transfer(sp[2]);
  spi.transfer(sp[1]);
  spi.transfer(sp[0]);
  spi.read(dest, size);
  spi.end();
  return ((int) size);
}

int 
S25FL127S::erase(uint32_t dest)
{
  // Write enable before page erase.
  spi.begin(this);
  spi.transfer(WREN);
  spi.end();

  // Use ERASE with 24-bit address; Big-endian
  uint8_t* dp = (uint8_t*) &dest;
  spi.begin(this);
  spi.transfer(P4E);
  spi.transfer(dp[2]);
  spi.transfer(dp[1]);
  spi.transfer(dp[0]);
  spi.end();

  // Wait for completion
  // Fix: Allow async erase
  while (!is_ready()) yield();

  // Return error code(-1) if erase error otherwise zero
  return (m_status.E_ERR ? -1 : 0);
}

int 
S25FL127S::write(uint32_t dest, const void* src, size_t size)
{
  // Write enable before page erase
  spi.begin(this);
  spi.transfer(WREN);
  spi.end();

  // Use PP with 24-bit address
  spi.begin(this);
  uint8_t* dp = (uint8_t*) &dest;
  spi.begin(this);
  spi.transfer(PP);
  spi.transfer(dp[2]);
  spi.transfer(dp[1]);
  spi.transfer(dp[0]);
  spi.write(src, size);
  spi.end();

  // Wait for completion
  // Fix: Allow async write
  while (!is_ready()) yield();

  // Return error code(-1) if write/program error otherwise size
  return (m_status.P_ERR ? -1 : (int) size);
}
